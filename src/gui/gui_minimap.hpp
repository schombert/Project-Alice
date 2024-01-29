#pragma once

#include "gui_element_types.hpp"
#include "gui_graphics.hpp"
#include "gui_ledger_window.hpp"
#include "gui_search_window.hpp"
#include "gui_main_menu.hpp"
#include "gui_message_filters_window.hpp"
#include "opengl_wrapper.hpp"
#include "map.hpp"
#include "map_modes.hpp"
#include <glm/glm.hpp>

namespace ui {

class minimap_mapmode_button : public checkbox_button {
public:
	void button_action(sys::state& state) noexcept override {
		map_mode::set_map_mode(state, target);
		// Update elements...
		state.ui_state.root->impl_on_update(state);
	}

	bool is_active(sys::state& state) noexcept override {
		return state.map_state.active_map_mode == target;
	}

	map_mode::mode target = map_mode::mode::terrain;

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		auto s = "mapmode_" + std::to_string(uint32_t(target));
		text::localised_format_box(state, contents, box, std::string_view(s));
		text::close_layout_box(contents, box);
	}
};

class minimap_goto_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(!state.ui_state.search_window) {
			auto window = make_element_by_type<province_search_window>(state, "goto_box");
			state.ui_state.search_window = window.get();
			state.ui_state.root->add_child_to_front(std::move(window));
		} else if(state.ui_state.search_window->is_visible()) {
			state.ui_state.search_window->set_visible(state, false);
		} else {
			state.ui_state.search_window->set_visible(state, true);
			state.ui_state.root->move_child_to_front(state.ui_state.search_window);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("goto_goto"));
		text::close_layout_box(contents, box);
	}
};

class minimap_ledger_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(!state.ui_state.ledger_window) {
			auto window = make_element_by_type<ledger_window>(state, "ledger");
			state.ui_state.ledger_window = window.get();
			state.ui_state.root->add_child_to_front(std::move(window));
		} else if(state.ui_state.ledger_window->is_visible()) {
			state.ui_state.ledger_window->set_visible(state, false);
		} else {
			state.ui_state.ledger_window->set_visible(state, true);
			state.ui_state.root->move_child_to_front(state.ui_state.ledger_window);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("m_ledger_button"));
		text::close_layout_box(contents, box);
	}
};

class macro_builder_template_name : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto index = retrieve<uint32_t>(state, parent);
		auto const& name = state.ui_state.templates[index].name;
		auto sv = std::string_view(name, name + sizeof(name));
		set_text(state, std::string(sv));
	}
};
class macro_builder_template_flag : public flag_button {
public:
	void on_update(sys::state& state) noexcept override {
		auto index = retrieve<uint32_t>(state, parent);
		auto nid = state.ui_state.templates[index].source;
		set_current_nation(state, state.world.nation_get_identity_from_identity_holder(nid));
	}
};
struct notify_template_select {};
class macro_builder_template_select : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		auto index = retrieve<uint32_t>(state, parent);
		if(index >= uint32_t(state.ui_state.templates.size()))
			return;
		std::memcpy(&state.ui_state.current_template, &state.ui_state.templates[index], sizeof(sys::macro_builder_template));
		send(state, parent, notify_template_select{});
	}
};
class macro_builder_template_entry : public listbox_row_element_base<uint32_t> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "name") {
			return make_element_by_type<macro_builder_template_name>(state, id);
		} else if(name == "shield") {
			return make_element_by_type<macro_builder_template_flag>(state, id);
		} else if(name == "background") {
			return make_element_by_type<macro_builder_template_select>(state, id);
		} else {
			return nullptr;
		}
	}
};
class macro_builder_template_listbox : public listbox_element_base<macro_builder_template_entry, uint32_t> {
protected:
	std::string_view get_row_element_name() override {
		return "alice_macro_builder_template_entry";
	}
public:
	void on_create(sys::state& state) noexcept override {
		listbox_element_base<macro_builder_template_entry, uint32_t>::on_create(state);
		auto sdir = simple_fs::get_or_create_templates_directory();
		auto f = simple_fs::open_file(sdir, state.loaded_scenario_file);
		if(f) {
			auto contents = simple_fs::view_contents(*f);
			if(contents.file_size > 0) {
				uint32_t num_templates = contents.file_size / sizeof(sys::macro_builder_template);
				//Corruption protection
				if(num_templates >= 8192 * 4)
					num_templates = 8192 * 4;
				state.ui_state.templates.resize(num_templates);
				std::memcpy(state.ui_state.templates.data(), contents.data, num_templates * sizeof(sys::macro_builder_template));
			}
		}
	}
	void on_update(sys::state& state) noexcept override {
		row_contents.resize(state.ui_state.templates.size(), 0);
		for(uint32_t i = 0; i < uint32_t(state.ui_state.templates.size()); i++)
			row_contents[i] = i;
		update(state);
	}
};

class macro_builder_unit_name : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::unit_type_id>(state, parent);
		auto name = text::produce_simple_string(state, state.military_definitions.unit_base_definitions[content].name);
		int32_t amount = state.ui_state.current_template.amounts[content.index()];
		set_text(state, "(" + std::to_string(amount) + ") " + name);
	}
};
class macro_builder_unit_button : public right_click_button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::unit_type_id>(state, parent);
		if(state.ui_state.current_template.amounts[content.index()] < 255) {
			state.ui_state.current_template.amounts[content.index()] += 1;
		}
		send(state, parent, notify_setting_update{});
	}
	void button_right_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::unit_type_id>(state, parent);
		if(state.ui_state.current_template.amounts[content.index()] > 0) {
			state.ui_state.current_template.amounts[content.index()] -= 1;
		}
		send(state, parent, notify_setting_update{});
	}
};
class macro_builder_unit_entry : public listbox_row_element_base<dcon::unit_type_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "name") {
			return make_element_by_type<macro_builder_unit_name>(state, id);
		} else if(name == "background") {
			return make_element_by_type<macro_builder_unit_button>(state, id);
		} else {
			return nullptr;
		}
	}
};
struct macro_builder_state {
	bool is_land;
};
class macro_builder_unit_listbox : public listbox_element_base<macro_builder_unit_entry, dcon::unit_type_id> {
protected:
	std::string_view get_row_element_name() override {
		return "alice_macro_builder_unit_entry";
	}
public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		bool is_land = retrieve<macro_builder_state>(state, parent).is_land;
		for(dcon::unit_type_id::value_base_t i = 0; i < state.military_definitions.unit_base_definitions.size(); i++) {
			if(state.military_definitions.unit_base_definitions[dcon::unit_type_id(i)].is_land == is_land) {
				row_contents.push_back(dcon::unit_type_id(i));
			}
		}
		update(state);
	}
};
class macro_builder_new_template_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		state.ui_state.current_template = sys::macro_builder_template{};
		std::memset(state.ui_state.current_template.name, ' ', sizeof(sys::macro_builder_template::name));
		send(state, parent, notify_setting_update{});
	}
};
class macro_builder_save_template_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		sys::macro_builder_template& t = state.ui_state.current_template;
		t.source = state.local_player_nation;
		// Replace templates with the same name and of the same scenario
		bool overwrite = false;
		for(auto& u : state.ui_state.templates) {
			if(std::memcmp(u.name, t.name, sizeof(sys::macro_builder_template::name)) == 0) {
				std::memcpy(&u, &t, sizeof(sys::macro_builder_template));
				overwrite = true;
				break;
			}
		}
		if(!overwrite) {
			state.ui_state.templates.push_back(t);
		}
		auto sdir = simple_fs::get_or_create_templates_directory();
		simple_fs::write_file(sdir, state.loaded_scenario_file, reinterpret_cast<const char*>(state.ui_state.templates.data()), uint32_t(state.ui_state.templates.size()) * sizeof(sys::macro_builder_template));
		send(state, parent, notify_setting_update{});
	}
};
class macro_builder_remove_template_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		sys::macro_builder_template& t = state.ui_state.current_template;
		for(uint32_t i = 0; i < uint32_t(state.ui_state.templates.size()); i++) {
			auto const& u = state.ui_state.templates[i];
			if(std::memcmp(u.name, t.name, sizeof(sys::macro_builder_template::name)) == 0) {
				state.ui_state.templates.erase(state.ui_state.templates.begin() + i);
				break;
			}
		}
		auto sdir = simple_fs::get_or_create_templates_directory();
		simple_fs::write_file(sdir, state.loaded_scenario_file, reinterpret_cast<const char*>(state.ui_state.templates.data()), uint32_t(state.ui_state.templates.size()) * sizeof(sys::macro_builder_template));
		send(state, parent, notify_setting_update{});
	}
};
struct notify_macro_toggle_is_land {};
class macro_builder_switch_type_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto is_land = retrieve<macro_builder_state>(state, parent).is_land;
		if(is_land) {
			set_button_text(state, text::produce_simple_string(state, "macro_switch_type_naval"));
		} else {
			set_button_text(state, text::produce_simple_string(state, "macro_switch_type_land"));
		}
	}
	void button_action(sys::state& state) noexcept override {
		send(state, parent, notify_macro_toggle_is_land{});
	}
};
class macro_builder_name_input : public edit_box_element_base {
public:
	void edit_box_update(sys::state& state, std::string_view str) noexcept override {
		auto s = parsers::remove_surrounding_whitespace(str);
		std::memset(state.ui_state.current_template.name, ' ', sizeof(sys::macro_builder_template::name));
		if(!s.empty())
			std::memcpy(state.ui_state.current_template.name, s.data(), std::min(s.length(), sizeof(sys::macro_builder_template::name)));
	}
};
class macro_builder_details : public scrollable_text {
public:
	void on_update(sys::state& state) noexcept override {
		auto contents = text::create_endless_layout(delegate->internal_layout,
				text::layout_parameters{ 0, 0, int16_t(base_data.size.x), int16_t(base_data.size.y),
			base_data.data.text.font_handle, 0, text::alignment::left,
			text::is_black_from_font_id(base_data.data.text.font_handle) ? text::text_color::black : text::text_color::white, false });
		auto is_land = retrieve<macro_builder_state>(state, parent).is_land;
		auto const& t = state.ui_state.current_template;

		float reconnaissance_or_fire_range = 0.f;
		float siege_or_torpedo_attack = 0.f;
		float attack_or_gun_power = 0.f;
		float defence_or_hull = 0.f;
		float discipline_or_evasion = std::numeric_limits<float>::max();
		float support = 0.f;
		float supply_consumption = 0.f;
		float maximum_speed = std::numeric_limits<float>::max();
		float maneuver = std::numeric_limits<float>::max();
		int32_t supply_consumption_score = 0;
		bool warn_overseas = false;
		bool warn_culture = false;
		bool warn_active = false;
		for(dcon::unit_type_id::value_base_t i = 0; i < sys::macro_builder_template::max_types; i++) {
			if(t.amounts[i] == 0) //not needed to show this
				continue;
			dcon::unit_type_id utid = dcon::unit_type_id(i);
			if(is_land != state.military_definitions.unit_base_definitions[utid].is_land)
				continue;

			if(!state.military_definitions.unit_base_definitions[utid].active && !state.world.nation_get_active_unit(state.local_player_nation, utid))
				warn_active = true;
			if(state.military_definitions.unit_base_definitions[utid].primary_culture)
				warn_culture = true;
			if(state.military_definitions.unit_base_definitions[utid].can_build_overseas)
				warn_overseas = true;

			reconnaissance_or_fire_range += state.world.nation_get_unit_stats(state.local_player_nation, utid).reconnaissance_or_fire_range * float(t.amounts[i]);
			siege_or_torpedo_attack += state.world.nation_get_unit_stats(state.local_player_nation, utid).siege_or_torpedo_attack * float(t.amounts[i]);
			attack_or_gun_power += state.world.nation_get_unit_stats(state.local_player_nation, utid).attack_or_gun_power * float(t.amounts[i]);
			defence_or_hull += state.world.nation_get_unit_stats(state.local_player_nation, utid).defence_or_hull * float(t.amounts[i]);
			discipline_or_evasion += std::min(discipline_or_evasion, state.world.nation_get_unit_stats(state.local_player_nation, utid).discipline_or_evasion);
			supply_consumption += state.world.nation_get_unit_stats(state.local_player_nation, utid).supply_consumption * float(t.amounts[i]);
			maximum_speed = std::min(maximum_speed, state.world.nation_get_unit_stats(state.local_player_nation, utid).maximum_speed);
			if(is_land) {
				support += state.world.nation_get_unit_stats(state.local_player_nation, utid).support * float(t.amounts[i]);
				maneuver += std::min(maneuver, state.military_definitions.unit_base_definitions[utid].maneuver);
			} else {
				supply_consumption_score += state.military_definitions.unit_base_definitions[utid].supply_consumption_score * int32_t(t.amounts[i]);
			}
		}

		if(warn_overseas)
			text::add_line(state, contents, "macro_warn_overseas");
		if(warn_culture)
			text::add_line(state, contents, "macro_warn_culture");
		if(warn_active)
			text::add_line(state, contents, "macro_warn_unlocked");

		// Total
		if(maximum_speed == std::numeric_limits<float>::max()) maximum_speed = 0.f;
		if(discipline_or_evasion == std::numeric_limits<float>::max()) discipline_or_evasion = 0.f;
		if(maneuver == std::numeric_limits<float>::max()) maneuver = 0.f;
		text::add_line(state, contents, text::produce_simple_string(state, "macro_total_desc"));
		if(is_land) {
			if(reconnaissance_or_fire_range > 0.f) {
				text::add_line(state, contents, "alice_recon", text::variable_type::x, text::format_float(reconnaissance_or_fire_range, 2));
			}
			if(siege_or_torpedo_attack > 0.f) {
				text::add_line(state, contents, "alice_siege", text::variable_type::x, text::format_float(siege_or_torpedo_attack, 2));
			}
			text::add_line(state, contents, "alice_attack", text::variable_type::x, text::format_float(attack_or_gun_power, 2));
			text::add_line(state, contents, "alice_defence", text::variable_type::x, text::format_float(defence_or_hull, 2));
			text::add_line(state, contents, "alice_discipline", text::variable_type::x, text::format_float(discipline_or_evasion * 100, 0));
			if(support > 0.f) {
				text::add_line(state, contents, "alice_support", text::variable_type::x, text::format_float(support, 0));
			}
			text::add_line(state, contents, "alice_maneuver", text::variable_type::x, text::format_float(maneuver, 0));
			text::add_line(state, contents, "alice_maximum_speed", text::variable_type::x, text::format_float(maximum_speed, 2));
			text::add_line(state, contents, "alice_supply_consumption", text::variable_type::x, text::format_float(supply_consumption * 100, 0));
		} else {
			text::add_line(state, contents, "alice_maximum_speed", text::variable_type::x, text::format_float(maximum_speed, 2));
			text::add_line(state, contents, "alice_attack", text::variable_type::x, text::format_float(attack_or_gun_power, 2));
			if(siege_or_torpedo_attack > 0.f) {
				text::add_line(state, contents, "alice_torpedo_attack", text::variable_type::x, text::format_float(siege_or_torpedo_attack, 2));
			}
			text::add_line(state, contents, "alice_hull", text::variable_type::x, text::format_float(defence_or_hull, 2));
			text::add_line(state, contents, "alice_fire_range", text::variable_type::x, text::format_float(reconnaissance_or_fire_range, 2));
			if(discipline_or_evasion > 0.f) {
				text::add_line(state, contents, "alice_evasion", text::variable_type::x, text::format_float(discipline_or_evasion * 100, 0));
			}
			text::add_line(state, contents, "alice_supply_consumption", text::variable_type::x, text::format_float(supply_consumption * 100, 0));
			text::add_line(state, contents, "alice_supply_load", text::variable_type::x, supply_consumption_score);
		}
		text::add_line_break_to_layout(state, contents);

		// Describe for each
		for(dcon::unit_type_id::value_base_t i = 0; i < sys::macro_builder_template::max_types; i++) {
			if(t.amounts[i] == 0) //not needed to show this
				continue;
			dcon::unit_type_id utid = dcon::unit_type_id(i);
			if(is_land != state.military_definitions.unit_base_definitions[utid].is_land)
				continue;
			text::add_line(state, contents, state.military_definitions.unit_base_definitions[utid].name);
			if(is_land) {
				if(state.world.nation_get_unit_stats(state.local_player_nation, utid).reconnaissance_or_fire_range > 0) {
					text::add_line(state, contents, "alice_recon", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).reconnaissance_or_fire_range * float(t.amounts[i]), 2));
				}
				if(state.world.nation_get_unit_stats(state.local_player_nation, utid).siege_or_torpedo_attack > 0) {
					text::add_line(state, contents, "alice_siege", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).siege_or_torpedo_attack * float(t.amounts[i]), 2));
				}
				text::add_line(state, contents, "alice_attack", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).attack_or_gun_power * float(t.amounts[i]), 2));
				text::add_line(state, contents, "alice_defence", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).defence_or_hull * float(t.amounts[i]), 2));
				text::add_line(state, contents, "alice_discipline", text::variable_type::x, text::format_float(state.military_definitions.unit_base_definitions[utid].discipline_or_evasion * 100 * float(t.amounts[i]), 0));
				if(state.military_definitions.unit_base_definitions[utid].support > 0.f) {
					text::add_line(state, contents, "alice_support", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).support * float(t.amounts[i]), 0));
				}
				text::add_line(state, contents, "alice_maneuver", text::variable_type::x, text::format_float(state.military_definitions.unit_base_definitions[utid].maneuver * float(t.amounts[i]), 0));
				text::add_line(state, contents, "alice_maximum_speed", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).maximum_speed * float(t.amounts[i]), 2));
				text::add_line(state, contents, "alice_supply_consumption", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).supply_consumption * 100 * float(t.amounts[i]), 0));
			} else {
				text::add_line(state, contents, "alice_maximum_speed", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).maximum_speed * float(t.amounts[i]), 2));
				text::add_line(state, contents, "alice_attack", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).attack_or_gun_power * float(t.amounts[i]), 2));
				if(state.world.nation_get_unit_stats(state.local_player_nation, utid).siege_or_torpedo_attack > 0) {
					text::add_line(state, contents, "alice_torpedo_attack", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).siege_or_torpedo_attack * float(t.amounts[i]), 2));
				}
				text::add_line(state, contents, "alice_hull", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).defence_or_hull* float(t.amounts[i]), 2));
				text::add_line(state, contents, "alice_fire_range", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).reconnaissance_or_fire_range* float(t.amounts[i]), 2));
				if(state.military_definitions.unit_base_definitions[utid].discipline_or_evasion > 0.f) {
					text::add_line(state, contents, "alice_evasion", text::variable_type::x, text::format_float(state.military_definitions.unit_base_definitions[utid].discipline_or_evasion * 100 * float(t.amounts[i]), 0));
				}
				text::add_line(state, contents, "alice_supply_consumption", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).supply_consumption * 100 * float(t.amounts[i]), 0));
				text::add_line(state, contents, "alice_supply_load", text::variable_type::x, state.military_definitions.unit_base_definitions[utid].supply_consumption_score * int32_t(t.amounts[i]));
			}
			text::add_line_break_to_layout(state, contents);
		}
		calibrate_scrollbar(state);
	}

	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		return message_result::consumed;
	}
};
class macro_builder_apply_button : public button_element_base {
	std::vector<dcon::province_id> provinces;
	std::vector<bool> marked;
	void get_provinces(sys::state& state, dcon::province_id p) {
		if(marked[p.index()])
			return;
		marked[p.index()] = true;
		if(state.world.province_get_nation_from_province_control(p) == state.local_player_nation && state.world.province_get_nation_from_province_ownership(p) == state.local_player_nation) {
			provinces.push_back(p);
			for(const auto adj : state.world.province_get_province_adjacency_as_connected_provinces(p)) {
				auto p2 = adj.get_connected_provinces(adj.get_connected_provinces(0) == p ? 1 : 0);
				get_provinces(state, p2);
			}
		}
	}
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		marked.resize(state.world.province_size() + 1, false);
	}
	void on_update(sys::state& state) noexcept override {
		disabled = (state.map_state.selected_province == dcon::province_id{});
	}
	void button_action(sys::state& state) noexcept override {
		auto is_land = retrieve<macro_builder_state>(state, parent).is_land;
		auto const& t = state.ui_state.current_template;
		const auto template_province = state.map_state.selected_province;
		uint8_t rem_to_build[sys::macro_builder_template::max_types];
		std::memcpy(rem_to_build, t.amounts, sizeof(rem_to_build));
		if(is_land) {
			provinces.clear();
			marked.assign(marked.size(), false);
			get_provinces(state, state.map_state.selected_province);
			if(provinces.empty())
				return;
			// Have to queue commands [temporarily on UI side] or it may mess calculations up
			struct build_queue_data {
				dcon::province_id p;
				dcon::culture_id c;
				dcon::unit_type_id u;
			};
			std::vector<build_queue_data> build_queue;
			for(const auto prov : provinces) {
				for(const auto p : state.world.province_get_pop_location_as_province(prov)) {
					if(p.get_pop().get_poptype() != state.culture_definitions.soldiers)
						continue;
					int32_t possible = military::regiments_possible_from_pop(state, p.get_pop());
					int32_t used = int32_t(p.get_pop().get_regiment_source().end() - p.get_pop().get_regiment_source().begin());
					int32_t avail = possible - used;
					if(possible > 0 && avail > 0) {
						for(dcon::unit_type_id::value_base_t i = 0; i < sys::macro_builder_template::max_types; i++) {
							dcon::unit_type_id utid = dcon::unit_type_id(i);
							if(rem_to_build[i] > 0
							&& is_land == state.military_definitions.unit_base_definitions[utid].is_land
							&& command::can_start_land_unit_construction(state, state.local_player_nation, prov, p.get_pop().get_culture(), utid, template_province)) {
								for(int32_t j = 0; j < int32_t(rem_to_build[i]) && j < avail; j++) {
									build_queue.push_back(build_queue_data{ prov, p.get_pop().get_culture(), utid });
									rem_to_build[i]--;
									avail--;
									if(rem_to_build[i] == 0)
										break;
								}
							}
						}
					}
				}
			}
			// Then flush them all!
			for(const auto& build : build_queue) {
				command::start_land_unit_construction(state, state.local_player_nation, build.p, build.c, build.u, template_province);
			}
			state.game_state_updated.store(true, std::memory_order::release);
		} else {
			provinces.clear();
			for(const auto pc : state.world.nation_get_province_ownership_as_nation(state.local_player_nation)) {
				if(pc.get_province().get_province_control().get_nation() == state.local_player_nation
				&& pc.get_province().get_is_coast()) {
					provinces.push_back(pc.get_province());
				}
			}
			if(provinces.empty())
				return;
			std::sort(provinces.begin(), provinces.end(), [&state](auto const a, auto const b) {
				auto ab = state.world.province_get_province_naval_construction_as_province(a);
				auto bb = state.world.province_get_province_naval_construction_as_province(b);
				int32_t asz = int32_t(ab.end() - ab.begin());
				int32_t bsz = int32_t(bb.end() - bb.begin());
				if(asz == bsz)
					return a.index() < b.index();
				return asz < bsz;
			});
			for(dcon::unit_type_id::value_base_t i = 0; i < sys::macro_builder_template::max_types; i++) {
				dcon::unit_type_id utid = dcon::unit_type_id(i);
				if(rem_to_build[i] > 0
				&& is_land == state.military_definitions.unit_base_definitions[utid].is_land
				&& (state.military_definitions.unit_base_definitions[utid].active || state.world.nation_get_active_unit(state.local_player_nation, utid))) {
					for(const auto prov : provinces) {
						auto const port_level = state.world.province_get_building_level(prov, economy::province_building_type::naval_base);
						if(port_level >= state.military_definitions.unit_base_definitions[utid].min_port_level
						&& command::can_start_naval_unit_construction(state, state.local_player_nation, prov, utid, template_province)) {
							command::start_naval_unit_construction(state, state.local_player_nation, prov, utid, template_province);
							rem_to_build[i]--;
							if(rem_to_build[i] == 0)
								break;
						}
					}
					// sort provinces again so that the ships can be built on parallel
					std::sort(provinces.begin(), provinces.end(), [&state](auto const a, auto const b) {
						auto ab = state.world.province_get_province_naval_construction_as_province(a);
						auto bb = state.world.province_get_province_naval_construction_as_province(b);
						int32_t asz = int32_t(ab.end() - ab.begin());
						int32_t bsz = int32_t(bb.end() - bb.begin());
						if(asz == bsz)
							return a.index() < b.index();
						return asz < bsz;
					});
				}
			}
			state.game_state_updated.store(true, std::memory_order::release);
		}
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(state.map_state.selected_province == dcon::province_id{}) {
			text::add_line(state, contents, "macro_select_province");
			return;
		}
		auto is_land = retrieve<macro_builder_state>(state, parent).is_land;
		auto const& t = state.ui_state.current_template;
		const auto template_province = state.map_state.selected_province;
		uint8_t rem_to_build[sys::macro_builder_template::max_types];
		std::memcpy(rem_to_build, t.amounts, sizeof(rem_to_build));
		if(is_land) {
			provinces.clear();
			marked.assign(marked.size(), false);
			get_provinces(state, state.map_state.selected_province);
			if(provinces.empty()) {
				text::add_line(state, contents, "macro_warn_invalid_province");
				return;
			}
			for(const auto prov : provinces) {
				for(const auto p : state.world.province_get_pop_location_as_province(prov)) {
					if(p.get_pop().get_poptype() != state.culture_definitions.soldiers)
						continue;
					int32_t possible = military::regiments_possible_from_pop(state, p.get_pop());
					int32_t used = int32_t(p.get_pop().get_regiment_source().end() - p.get_pop().get_regiment_source().begin());
					int32_t avail = possible - used;
					if(possible > 0 && avail > 0) {
						for(dcon::unit_type_id::value_base_t i = 0; i < sys::macro_builder_template::max_types; i++) {
							dcon::unit_type_id utid = dcon::unit_type_id(i);
							if(rem_to_build[i] > 0
							&& is_land == state.military_definitions.unit_base_definitions[utid].is_land
							&& command::can_start_land_unit_construction(state, state.local_player_nation, prov, p.get_pop().get_culture(), utid, template_province)) {
								for(int32_t j = 0; j < int32_t(rem_to_build[i]) && j < avail; j++) {
									rem_to_build[i]--;
									avail--;
									if(rem_to_build[i] == 0)
										break;
								}
							}
						}
					}
				}
			}
		} else {
			provinces.clear();
			for(const auto pc : state.world.nation_get_province_ownership_as_nation(state.local_player_nation)) {
				if(pc.get_province().get_province_control().get_nation() == state.local_player_nation
				&& pc.get_province().get_is_coast()) {
					provinces.push_back(pc.get_province());
				}
			}
			if(provinces.empty()) {
				text::add_line(state, contents, "macro_warn_invalid_province");
				return;
			}
			std::sort(provinces.begin(), provinces.end(), [&state](auto const a, auto const b) {
				auto ab = state.world.province_get_province_naval_construction_as_province(a);
				auto bb = state.world.province_get_province_naval_construction_as_province(b);
				int32_t asz = int32_t(ab.end() - ab.begin());
				int32_t bsz = int32_t(bb.end() - bb.begin());
				if(asz == bsz)
					return a.index() < b.index();
				return asz < bsz;
			});
			for(dcon::unit_type_id::value_base_t i = 0; i < sys::macro_builder_template::max_types; i++) {
				dcon::unit_type_id utid = dcon::unit_type_id(i);
				if(rem_to_build[i] > 0
				&& is_land == state.military_definitions.unit_base_definitions[utid].is_land
				&& (state.military_definitions.unit_base_definitions[utid].active || state.world.nation_get_active_unit(state.local_player_nation, utid))) {
					for(const auto prov : provinces) {
						auto const port_level = state.world.province_get_building_level(prov, economy::province_building_type::naval_base);
						if(port_level >= state.military_definitions.unit_base_definitions[utid].min_port_level
						&& command::can_start_naval_unit_construction(state, state.local_player_nation, prov, utid, template_province)) {
							rem_to_build[i]--;
							if(rem_to_build[i] == 0)
								break;
						}
					}
					// sort provinces again so that the ships can be built on parallel
					std::sort(provinces.begin(), provinces.end(), [&state](auto const a, auto const b) {
						auto ab = state.world.province_get_province_naval_construction_as_province(a);
						auto bb = state.world.province_get_province_naval_construction_as_province(b);
						int32_t asz = int32_t(ab.end() - ab.begin());
						int32_t bsz = int32_t(bb.end() - bb.begin());
						if(asz == bsz)
							return a.index() < b.index();
						return asz < bsz;
					});
				}
			}
		}

		for(dcon::unit_type_id::value_base_t i = 0; i < sys::macro_builder_template::max_types; i++) {
			if(rem_to_build[i] > 0) {
				dcon::unit_type_id utid = dcon::unit_type_id(i);
				text::substitution_map sub{};
				text::add_to_substitution_map(sub, text::variable_type::x, text::int_wholenum{ t.amounts[i] });
				text::add_to_substitution_map(sub, text::variable_type::y, text::int_wholenum{ t.amounts[i] - rem_to_build[i] });
				text::add_to_substitution_map(sub, text::variable_type::name, state.military_definitions.unit_base_definitions[utid].name);
				auto box = text::open_layout_box(contents);
				text::localised_format_box(state, contents, box, "macro_warn_insuff", sub);
				text::close_layout_box(contents, box);
			}
		}
	}
};
class macro_builder_window : public window_element_base {
	bool is_land = true;
	macro_builder_name_input* name_input = nullptr;
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		impl_on_update(state);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "background") {
			return make_element_by_type<draggable_target>(state, id);
		} else if(name == "close") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "input") {
			auto ptr = make_element_by_type<macro_builder_name_input>(state, id);
			name_input = ptr.get();
			return ptr;
		} else if(name == "template_listbox") {
			return make_element_by_type<macro_builder_template_listbox>(state, id);
		} else if(name == "unit_listbox") {
			return make_element_by_type<macro_builder_unit_listbox>(state, id);
		} else if(name == "new_template") {
			return make_element_by_type<macro_builder_new_template_button>(state, id);
		} else if(name == "save_template") {
			return make_element_by_type<macro_builder_save_template_button>(state, id);
		} else if(name == "remove_template") {
			return make_element_by_type<macro_builder_remove_template_button>(state, id);
		} else if(name == "switch_type") {
			return make_element_by_type<macro_builder_switch_type_button>(state, id);
		} else if(name == "apply") {
			return make_element_by_type<macro_builder_apply_button>(state, id);
		} else if(name == "details") {
			return make_element_by_type<macro_builder_details>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<macro_builder_state>()) {
			payload.emplace<macro_builder_state>(macro_builder_state{ is_land });
			return message_result::consumed;
		} else if(payload.holds_type< notify_macro_toggle_is_land>()) {
			is_land = !is_land;
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type< notify_template_select>()) {
			auto const& name = state.ui_state.current_template.name;
			auto sv = std::string_view(name, name + sizeof(name));
			auto s = std::string(sv);
			name_input->edit_index_position(state, 0);
			name_input->set_text(state, s);
			impl_on_update(state);
			return message_result::consumed;
		}  else if(payload.holds_type<notify_setting_update>()) {
			impl_on_update(state);
			return message_result::consumed;
		}
		return message_result::consumed;// window_element_base::impl_get(state, payload);
	}
};
class minimap_macro_builder_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(!state.ui_state.macro_builder_window) {
			auto window = make_element_by_type<macro_builder_window>(state, "alice_macro_builder");
			state.ui_state.macro_builder_window = window.get();
			state.ui_state.root->add_child_to_front(std::move(window));
		} else if(state.ui_state.macro_builder_window->is_visible()) {
			state.ui_state.macro_builder_window->set_visible(state, false);
		} else {
			state.ui_state.macro_builder_window->set_visible(state, true);
			state.ui_state.root->move_child_to_front(state.ui_state.macro_builder_window);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("macro_builder"));
		text::close_layout_box(contents, box);
	}
};

class minimap_msg_settings_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(!state.ui_state.msg_filters_window) {
			auto window = make_element_by_type<message_filters_window>(state, "message_filters");
			state.ui_state.msg_filters_window = window.get();
			state.ui_state.root->add_child_to_front(std::move(window));
		} else if(state.ui_state.msg_filters_window->is_visible()) {
			state.ui_state.msg_filters_window->set_visible(state, false);
		} else {
			state.ui_state.msg_filters_window->set_visible(state, true);
			state.ui_state.root->move_child_to_front(state.ui_state.msg_filters_window);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("sidemenu_menubar_msg_settings"), text::substitution_map{});
		text::close_layout_box(contents, box);
	}
};

class minimap_msg_combat_button : public button_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("sidemenu_menubar_msg_combat"), text::substitution_map{});
		text::close_layout_box(contents, box);
	}
};

class minimap_msg_diplo_button : public button_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("sidemenu_menubar_msg_diplo"), text::substitution_map{});
		text::close_layout_box(contents, box);
	}
};

class minimap_msg_unit_button : public button_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("sidemenu_menubar_msg_unit"), text::substitution_map{});
		text::close_layout_box(contents, box);
	}
};

class minimap_msg_province_button : public button_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("sidemenu_menubar_msg_province"), text::substitution_map{});
		text::close_layout_box(contents, box);
	}
};

class minimap_msg_other_button : public button_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("sidemenu_menubar_msg_other"), text::substitution_map{});
		text::close_layout_box(contents, box);
	}
};

class minimap_msg_event_button : public button_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("sidemenu_menubar_msg_event"), text::substitution_map{});
		text::close_layout_box(contents, box);
	}
};

class minimap_menu_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(!state.ui_state.main_menu) {
			auto window = make_element_by_type<main_menu_window>(state, "alice_main_menu");
			state.ui_state.main_menu = window.get();
			state.ui_state.root->add_child_to_front(std::move(window));
		} else if(state.ui_state.main_menu->is_visible()) {
			state.ui_state.main_menu->set_visible(state, false);
		} else {
			state.ui_state.main_menu->set_visible(state, true);
			state.ui_state.root->move_child_to_front(state.ui_state.main_menu);
		}
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("m_menu_button"));
		text::close_layout_box(contents, box);
	}
};

struct open_msg_log_data {};

class open_msg_log_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		frame = state.ui_state.msg_log_window->is_visible() ? 1 : 0;
	}

	void button_action(sys::state& state) noexcept override {
		send(state, parent, open_msg_log_data{});
	}
};

class minimap_zoom_in_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		state.map_state.zoom = std::clamp(state.map_state.zoom * 2.0f,map::min_zoom, map::max_zoom);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "zoom_in");
	}
};
class minimap_zoom_out_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		state.map_state.zoom = std::clamp(state.map_state.zoom * 0.5f, map::min_zoom, map::max_zoom);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t t, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "zoom_out");
	}
};

class minimap_container_window : public window_element_base {
	const std::string_view mapmode_btn_prefix{"mapmode_"};

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "messagelog_window") {
			auto ptr = make_element_by_type<message_log_window>(state, id);
			ptr->base_data.position.y += 1; //nudge
			state.ui_state.msg_log_window = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "minimap_bg") {
			return make_element_by_type<opaque_element_base>(state, id);
		} else if(name == "openbutton") {
			auto ptr = make_element_by_type<open_msg_log_button>(state, id);
			ptr->base_data.position.y += 1; //nudge
			return ptr;
		} else if(name == "menu_button") {
			return make_element_by_type<minimap_menu_button>(state, id);
		} else if(name == "button_goto") {
			auto ptr = make_element_by_type<minimap_macro_builder_button>(state, id);
			ptr->base_data.position.y += ptr->base_data.size.y;
			add_child_to_front(std::move(ptr));
			return make_element_by_type<minimap_goto_button>(state, id);
		} else if(name == "ledger_button") {
			return make_element_by_type<minimap_ledger_button>(state, id);
		} else if(name == "map_zoom_in") {
			return make_element_by_type<minimap_zoom_in_button>(state, id);
		} else if(name == "map_zoom_out") {
			return make_element_by_type<minimap_zoom_out_button>(state, id);
		} else if(name == "menubar_bg") {
			return partially_transparent_image::make_element_by_type_alias(state, id);
		} else if(name == "menubar_bg") {
			return partially_transparent_image::make_element_by_type_alias(state, id);
		} else if(name == "chat_window"
			|| name == "menubar_mail_bg"
			|| name == "menubar_msg_settings"
			|| name == "menubar_msg_combat"
			|| name == "menubar_msg_diplo"
			|| name == "menubar_msg_unit"
			|| name == "menubar_msg_province"
			|| name == "menubar_msg_event"
			|| name == "menubar_msg_other"
			|| name == "menubar_plans_toggle"
			|| name == "menubar_plans_open") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name.starts_with(mapmode_btn_prefix)) {
			auto ptr = make_element_by_type<minimap_mapmode_button>(state, id);
			size_t num_index = name.rfind("_") + 1;
			uint8_t num = 0;
			for(size_t i = num_index; i < name.size(); i++) {
				num *= 10;
				num += name[i] - '0';
			}
			ptr->target = static_cast<map_mode::mode>(num);
			return ptr;
		} else {
			return nullptr;
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		base_data.size.x = int16_t(ui_width(state));
		base_data.size.y = int16_t(ui_height(state));
		window_element_base::render(state, x, y);
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<open_msg_log_data>()) {
			state.ui_state.msg_log_window->set_visible(state, !state.ui_state.msg_log_window->is_visible());
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class minimap_picture_window : public opaque_element_base {
public:
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		opaque_element_base::render(state, x, y);
		// TODO draw white box to represent window borders
	}

	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		auto minimap_size = glm::vec2(base_data.size.x, base_data.size.y);
		state.map_state.set_pos(glm::vec2(x, y) / minimap_size);
		return message_result::consumed;
	}

	message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override {
		return message_result::unseen;
	}
};

} // namespace ui
