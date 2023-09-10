#pragma once

#include "gui_element_types.hpp"

namespace ui {

class buildable_unit_entry_info {
public:
	dcon::pop_id pop_info;
	dcon::province_id province_info;
	// false == army
	// true == navy
	bool is_navy;
};

class queue_unit_entry_info {
public:
	dcon::province_land_construction_id land_id;
	dcon::province_naval_construction_id naval_id;
	// false == army
	// true == navy
	bool is_navy;
};

class build_unit_close_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		state.ui_state.unit_window_army->set_visible(state, true);
		state.ui_state.unit_window_navy->set_visible(state, true);

		state.ui_state.build_unit_window->set_visible(state, false);
	}
};

class unit_build_button : public button_element_base {
public:
	bool is_navy = false;
	void button_action(sys::state& state) noexcept override {
		dcon::nation_id n = retrieve<dcon::nation_id>(state, parent);
		dcon::unit_type_id utid = retrieve<dcon::unit_type_id>(state, parent);
		dcon::province_id p = retrieve<dcon::province_id>(state, parent);

		if(is_navy == false) {
			dcon::culture_id c = retrieve<dcon::culture_id>(state, parent);
			command::start_land_unit_construction(state, n, p, c, utid);
		} else {
			command::start_naval_unit_construction(state, n, p, utid);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		dcon::unit_type_id utid = retrieve<dcon::unit_type_id>(state, parent);
		dcon::province_id p = retrieve<dcon::province_id>(state, parent);
		if(is_navy) {
			text::add_line(state, contents, "military_build_unit_tooltip", text::variable_type::name, state.military_definitions.unit_base_definitions[utid].name, text::variable_type::loc, state.world.province_get_name(p));
			//Any key starting with 'alice' has been added in \assets\alice.csv
			text::add_line(state, contents, "alice_maximum_speed", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).maximum_speed, 2));
			text::add_line(state, contents, "alice_attack", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).attack_or_gun_power, 2));
			if(state.world.nation_get_unit_stats(state.local_player_nation, utid).siege_or_torpedo_attack > 0) {
				text::add_line(state, contents, "alice_torpedo_attack", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).siege_or_torpedo_attack, 2));
			}
			text::add_line(state, contents, "alice_hull", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).defence_or_hull, 2));
			text::add_line(state, contents, "alice_fire_range", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).reconnaissance_or_fire_range, 2));
			if(state.military_definitions.unit_base_definitions[utid].maneuver_or_evasion > 0) {
				text::add_line(state, contents, "alice_evasion", text::variable_type::x, text::format_float(state.military_definitions.unit_base_definitions[utid].maneuver_or_evasion * 100, 0));
			}
			text::add_line(state, contents, "alice_supply_consumption", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).supply_consumption*100, 0));
			text::add_line(state, contents, "alice_supply_load", text::variable_type::x, state.military_definitions.unit_base_definitions[utid].supply_consumption_score);
		} else {
			text::add_line(state, contents, "military_build_unit_tooltip", text::variable_type::name, state.military_definitions.unit_base_definitions[utid].name, text::variable_type::loc, state.world.province_get_name(p));
			if(state.world.nation_get_unit_stats(state.local_player_nation, utid).reconnaissance_or_fire_range > 0) {
				text::add_line(state, contents, "alice_recon", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).reconnaissance_or_fire_range, 2));
			}
			if(state.world.nation_get_unit_stats(state.local_player_nation, utid).siege_or_torpedo_attack > 0) {
				text::add_line(state, contents, "alice_siege", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).siege_or_torpedo_attack, 2));
			}
			text::add_line(state, contents, "alice_attack", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).attack_or_gun_power, 2));
			text::add_line(state, contents, "alice_defence", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).defence_or_hull, 2));
			text::add_line(state, contents, "alice_discipline", text::variable_type::x, text::format_float(state.military_definitions.unit_base_definitions[utid].discipline*100, 0));
			if(state.military_definitions.unit_base_definitions[utid].support > 0) {
				text::add_line(state, contents, "alice_support", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).support * 100, 0));
			}
			text::add_line(state, contents, "alice_maneuver", text::variable_type::x, text::format_float(state.military_definitions.unit_base_definitions[utid].maneuver_or_evasion, 0));
			text::add_line(state, contents, "alice_maximum_speed", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).maximum_speed, 2));
			text::add_line(state, contents, "alice_supply_consumption", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).supply_consumption * 100, 0));
		}
	}
};

class unit_queue_button : public button_element_base {
public:
	bool is_navy = false;
	void button_action(sys::state& state) noexcept override {
		ui::queue_unit_entry_info local_unit_entry = retrieve<ui::queue_unit_entry_info>(state, parent);
		if(!local_unit_entry.is_navy) {
			dcon::nation_id n = state.world.province_land_construction_get_nation(local_unit_entry.land_id);
			dcon::unit_type_id utid = state.world.province_land_construction_get_type(local_unit_entry.land_id);
			dcon::province_id p = state.world.pop_get_province_from_pop_location(state.world.province_land_construction_get_pop(local_unit_entry.land_id));
			dcon::culture_id c = state.world.pop_get_culture(state.world.province_land_construction_get_pop(local_unit_entry.land_id));
			command::cancel_land_unit_construction(state, n, p, c, utid);
		} else {
			dcon::nation_id n = state.world.province_naval_construction_get_nation(local_unit_entry.naval_id);
			dcon::unit_type_id utid = state.world.province_naval_construction_get_type(local_unit_entry.naval_id);
			dcon::province_id p = state.world.province_naval_construction_get_province(local_unit_entry.naval_id);
			command::cancel_naval_unit_construction(state, n, p, utid);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		/*
		if(is_navy) {
			Cyto::Any payload = dcon::unit_type_id{};
			parent->impl_get(state, payload);
			dcon::unit_type_id utid = Cyto::any_cast<dcon::unit_type_id>(payload);
			Cyto::Any p_payload = dcon::province_id{};
			parent->impl_get(state, p_payload);
			dcon::province_id p = Cyto::any_cast<dcon::province_id>(p_payload);
			text::add_line(state, contents, "military_build_unit_tooltip", text::variable_type::name, state.military_definitions.unit_base_definitions[utid].name, text::variable_type::loc, state.world.province_get_name(p));
			//Any key starting with 'alice' has been added in \assets\alice.csv
			text::add_line(state, contents, "alice_maximum_speed", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).maximum_speed, 2));
			text::add_line(state, contents, "alice_attack", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).attack_or_gun_power, 2));
			text::add_line(state, contents, "alice_hull", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).defence_or_hull, 2));
			text::add_line(state, contents, "alice_fire_range", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).reconnaissance_or_fire_range, 2));
			text::add_line(state, contents, "alice_supply_consumption", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).supply_consumption * 100, 0));
			text::add_line(state, contents, "alice_supply_load", text::variable_type::x, state.military_definitions.unit_base_definitions[utid].supply_consumption_score);
		} else {
			Cyto::Any payload = dcon::unit_type_id{};
			parent->impl_get(state, payload);
			dcon::unit_type_id utid = Cyto::any_cast<dcon::unit_type_id>(payload);
			Cyto::Any p_payload = dcon::province_id{};
			parent->impl_get(state, p_payload);
			dcon::province_id p = Cyto::any_cast<dcon::province_id>(p_payload);
			text::add_line(state, contents, "military_build_unit_tooltip", text::variable_type::name, state.military_definitions.unit_base_definitions[utid].name, text::variable_type::loc, state.world.province_get_name(p));
			text::add_line(state, contents, "alice_attack", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).attack_or_gun_power, 2));
			text::add_line(state, contents, "alice_defence", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).defence_or_hull, 2));
			text::add_line(state, contents, "alice_discipline", text::variable_type::x, text::format_float(state.military_definitions.unit_base_definitions[utid].discipline * 100, 0));
			text::add_line(state, contents, "alice_maneuver", text::variable_type::x, text::format_float(state.military_definitions.unit_base_definitions[utid].maneuver_or_evasion, 0));
			text::add_line(state, contents, "alice_maximum_speed", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).maximum_speed, 2));
			text::add_line(state, contents, "alice_supply_consumption", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, utid).supply_consumption * 100, 0));
		}
		*/
	}
};

class unit_folder_button : public button_element_base {
public:
	dcon::unit_type_id unit_type{};

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<dcon::unit_type_id>{unit_type};
			parent->impl_get(state, payload);
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			dcon::nation_id n = Cyto::any_cast<dcon::nation_id>(payload);
			disabled = state.world.nation_get_active_unit(n, unit_type) == false &&
								 state.military_definitions.unit_base_definitions[unit_type].active == false;
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(state.military_definitions.unit_base_definitions[unit_type].is_land) {
			text::add_line(state, contents, "alice_unit_folder_unit_name", text::variable_type::x, state.military_definitions.unit_base_definitions[unit_type].name);
			text::add_line(state, contents, " ");
			if(state.world.nation_get_unit_stats(state.local_player_nation, unit_type).reconnaissance_or_fire_range > 0) {
				text::add_line(state, contents, "alice_recon", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, unit_type).reconnaissance_or_fire_range, 2));
			}
			if(state.world.nation_get_unit_stats(state.local_player_nation, unit_type).siege_or_torpedo_attack > 0) {
				text::add_line(state, contents, "alice_siege", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, unit_type).siege_or_torpedo_attack, 2));
			}
			text::add_line(state, contents, "alice_attack", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, unit_type).attack_or_gun_power, 2));
			text::add_line(state, contents, "alice_defence", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, unit_type).defence_or_hull, 2));
			text::add_line(state, contents, "alice_discipline", text::variable_type::x, text::format_float(state.military_definitions.unit_base_definitions[unit_type].discipline * 100, 0));
			if(state.military_definitions.unit_base_definitions[unit_type].support > 0) {
				text::add_line(state, contents, "alice_support", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, unit_type).support * 100, 0));
			}
			text::add_line(state, contents, "alice_maneuver", text::variable_type::x, text::format_float(state.military_definitions.unit_base_definitions[unit_type].maneuver_or_evasion, 0));
			text::add_line(state, contents, "alice_maximum_speed", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, unit_type).maximum_speed, 2));
			text::add_line(state, contents, "alice_supply_consumption", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, unit_type).supply_consumption * 100, 0));
		} else {
			text::add_line(state, contents, "alice_unit_folder_unit_name", text::variable_type::x, state.military_definitions.unit_base_definitions[unit_type].name);
			text::add_line(state, contents, " ");
			text::add_line(state, contents, "alice_maximum_speed", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, unit_type).maximum_speed, 2));
			text::add_line(state, contents, "alice_attack", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, unit_type).attack_or_gun_power, 2));
			if(state.world.nation_get_unit_stats(state.local_player_nation, unit_type).siege_or_torpedo_attack > 0) {
				text::add_line(state, contents, "alice_torpedo_attack", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, unit_type).siege_or_torpedo_attack, 2));
			}
			text::add_line(state, contents, "alice_hull", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, unit_type).defence_or_hull, 2));
			text::add_line(state, contents, "alice_fire_range", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, unit_type).reconnaissance_or_fire_range, 2));
			if(state.military_definitions.unit_base_definitions[unit_type].maneuver_or_evasion > 0) {
				text::add_line(state, contents, "alice_evasion", text::variable_type::x, text::format_float(state.military_definitions.unit_base_definitions[unit_type].maneuver_or_evasion * 100, 0));
			}
			text::add_line(state, contents, "alice_supply_consumption", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, unit_type).supply_consumption * 100, 0));
			text::add_line(state, contents, "alice_supply_load", text::variable_type::x, state.military_definitions.unit_base_definitions[unit_type].supply_consumption_score);
		}
	}

};

class units_build_item : public listbox_row_element_base<buildable_unit_entry_info> {
public:
	ui::unit_build_button* build_button;
	ui::simple_text_element_base* unit_name = nullptr;
	ui::image_element_base* unit_icon = nullptr;
	ui::simple_text_element_base* build_time = nullptr;
	ui::simple_text_element_base* pop_size = nullptr;
	ui::simple_text_element_base* brigades = nullptr;

	std::string pop_size_text;

	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base::on_create(state);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "build_button") {
			auto ptr = make_element_by_type<unit_build_button>(state, id);
			ptr->set_button_text(state, "");
			build_button = ptr.get();
			return ptr;
		} else if(name == "build_button_group") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			unit_name = ptr.get();
			return ptr;
		} else if(name == "unit_strip") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			unit_icon = ptr.get();
			return ptr;
		} else if(name == "province") {
			auto ptr = make_element_by_type<generic_name_text<dcon::province_id>>(state, id);
			return ptr;
		} else if(name == "time_to_build") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			build_time = ptr.get();
			return ptr;
		} else if(name == "popsize") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			pop_size = ptr.get();
			auto ptr2 = make_element_by_type<simple_text_element_base>(state, id);
			ptr2->base_data.position.y += 14;
			brigades = ptr2.get();
			add_child_to_back(std::move(ptr2));
			return ptr;
		} else {
			return nullptr;
		}
	}

	void update(sys::state& state) noexcept override {
		dcon::unit_type_id utid = retrieve<dcon::unit_type_id>(state, parent);
		//unit_strip
		unit_icon->frame = int32_t(state.military_definitions.unit_base_definitions[utid].icon - 1);
		//time_to_build
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::x, state.military_definitions.unit_base_definitions[utid].build_time);
		build_time->set_text(state, text::resolve_string_substitution(state, "alice_build_time", m));
		//popsize
		if(content.is_navy) {
			pop_size->set_text(state, "");
			brigades->set_text(state, "");
		} else {
			pop_size_text = text::format_float(state.world.pop_get_size(content.pop_info) / 1000, 1);
			int num_of_brigades = 0;

			state.world.pop_get_province_from_pop_location(content.pop_info);

			for(auto pl : state.world.province_get_pop_location_as_province(state.world.pop_get_province_from_pop_location(content.pop_info))) {
				if(pl.get_pop().get_poptype() == state.culture_definitions.soldiers) {
					for(auto i : state.world.pop_get_regiment_source(pl.get_pop())) {
						num_of_brigades += 1;
					}
				}
			}

			text::substitution_map n;
			text::add_to_substitution_map(n, text::variable_type::x, std::string_view(pop_size_text));
			pop_size->set_text(state, text::resolve_string_substitution(state, "alice_build_unit_pop_size", n));

			text::substitution_map p;
			text::add_to_substitution_map(p, text::variable_type::x, num_of_brigades);
			text::add_to_substitution_map(p, text::variable_type::y, military::regiments_max_possible_from_province(state, state.world.pop_get_province_from_pop_location(content.pop_info)));
			brigades->set_text(state, text::resolve_string_substitution(state, "alice_build_unit_brigades", p));
		}

		//name
		build_button->is_navy = content.is_navy;
		if(content.is_navy == false) {
			build_button->frame = 0;
			auto culture_id = state.world.pop_get_culture(content.pop_info);
			auto culture_content = text::produce_simple_string(state, culture_id.get_name());
			auto unit_type_name = text::produce_simple_string(state, state.military_definitions.unit_base_definitions[utid].name);
			unit_name->set_text(state, culture_content + " " + unit_type_name);
		} else {
			build_button->frame = 1;
			unit_name->set_text(state, text::produce_simple_string(state, state.military_definitions.unit_base_definitions[utid].name));
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::culture_id>()) {
			payload.emplace<dcon::culture_id>(state.world.pop_get_culture(content.pop_info).id);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::province_id>()) {
			payload.emplace<dcon::province_id>(content.province_info);
			return message_result::consumed;
		}
		return listbox_row_element_base<buildable_unit_entry_info>::get(state, payload);
	}
};

class units_build_listbox : public listbox_element_base<units_build_item, buildable_unit_entry_info> {
protected:
	std::string_view get_row_element_name() override {
		return "build_unit_entry_wide";
	}

public:
	// false == army
	// true == navy
	bool is_navy = true;

	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		dcon::unit_type_id utid = retrieve<dcon::unit_type_id>(state, parent);
		if(is_navy == false) {
			for(auto po : state.world.nation_get_province_ownership_as_nation(state.local_player_nation)) {
				auto p = po.get_province();
				state.world.for_each_culture([&](dcon::culture_id c) {
					if(command::can_start_land_unit_construction(state, state.local_player_nation, p, c, utid)) {
						buildable_unit_entry_info info;
						info.is_navy = false;
						for(auto pl : state.world.province_get_pop_location_as_province(p)) {
							if(pl.get_pop().get_culture() == c) {
								if(pl.get_pop().get_poptype() == state.culture_definitions.soldiers && state.world.pop_get_size(pl.get_pop()) >= 1000.0f) {
									info.pop_info = pl.get_pop();
									break;
								}
							}
						}
						info.province_info = p;
						row_contents.push_back(info);
					}
				});
			}
		} else {
			for(auto po : state.world.nation_get_province_ownership_as_nation(state.local_player_nation)) {
				auto p = po.get_province();
				if(command::can_start_naval_unit_construction(state, state.local_player_nation, p, utid)) {
					buildable_unit_entry_info info;
					info.is_navy = true;
					info.pop_info = dcon::pop_id(1);
					info.province_info = p;
					row_contents.push_back(info);
				}
			}
		}
		update(state);
	}
};

class units_queue_item : public listbox_row_element_base<queue_unit_entry_info> {
public:
	ui::image_element_base* unit_icon = nullptr;
	ui::simple_text_element_base* unit_name = nullptr;
	ui::unit_queue_button* queue_button = nullptr;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "button") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "button2") {
			auto ptr = make_element_by_type<ui::unit_queue_button>(state, id);
			ptr->set_button_text(state, "");
			queue_button = ptr.get();
			return ptr;
		} else if(name == "unit_strip") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			unit_icon = ptr.get();
			return ptr;
		} else if(name == "name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			unit_name = ptr.get();
			return ptr;
		} else if(name == "province") {
			return make_element_by_type<generic_name_text<dcon::province_id>>(state, id);
		} else if(name == "continent") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else {
			return nullptr;
		}
	}

	void update(sys::state& state) noexcept override {
		if(!content.is_navy) {
			auto c = content.land_id;

			dcon::unit_type_id utid = state.world.province_land_construction_get_type(c);
			unit_icon->frame = state.military_definitions.unit_base_definitions[utid].icon - 1;

			auto culture_content = text::produce_simple_string(state,
					state.world.pop_get_culture(state.world.province_land_construction_get_pop(c)).get_name());
			auto unit_type_name = text::produce_simple_string(state, state.military_definitions.unit_base_definitions[utid].name);
			unit_name->set_text(state, culture_content + " " + unit_type_name);
			queue_button->is_navy = false;
		} else if(content.is_navy) {
			auto c = content.naval_id;

			dcon::unit_type_id utid = state.world.province_naval_construction_get_type(c);
			unit_icon->frame = state.military_definitions.unit_base_definitions[utid].icon - 1;

			unit_name->set_text(state, text::produce_simple_string(state, state.military_definitions.unit_base_definitions[utid].name));
			queue_button->is_navy = true;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {

		if(payload.holds_type<dcon::province_id>()) {
			dcon::province_id p{};
			if(!content.is_navy) {
				auto c = content.land_id;
				p = state.world.pop_location_get_province(state.world.pop_get_pop_location_as_pop(state.world.province_land_construction_get_pop(c)));
			} else if(content.is_navy) {
				auto c = content.naval_id;
				p = state.world.province_naval_construction_get_province(c);
			}
			payload.emplace<dcon::province_id>(p);
			return message_result::consumed;
		} else if(payload.holds_type<ui::queue_unit_entry_info>()) {
			payload.emplace<ui::queue_unit_entry_info>(content);
			return message_result::consumed;
		}
		return listbox_row_element_base<queue_unit_entry_info>::get(state, payload);
	}
};

class units_queue_listbox : public listbox_element_base<units_queue_item, ui::queue_unit_entry_info> {
protected:
	std::string_view get_row_element_name() override {
		return "queue_unit_entry";
	}

public:
	// false == army
	// true == navy
	bool is_navy = true;

	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		if(is_navy == false) {
			state.world.nation_for_each_province_land_construction_as_nation(state.local_player_nation, [&](dcon::province_land_construction_id c) { row_contents.push_back(ui::queue_unit_entry_info{c, dcon::province_naval_construction_id{0}, false}); });
		} else {
			state.world.nation_for_each_province_naval_construction_as_nation(state.local_player_nation, [&](dcon::province_naval_construction_id c) { row_contents.push_back(ui::queue_unit_entry_info{dcon::province_land_construction_id{0}, c, true}); });
		}
		update(state);
	}
};

class build_unit_large_window : public window_element_base {
	dcon::unit_type_id unit_type{};
	dcon::unit_type_id first_land_type{};
	dcon::unit_type_id first_naval_type{};

public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "cancel_all_units") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			return ptr;
		} else if(name == "build_army_label") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			army_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "build_navy_label") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			navy_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "close") {
			auto ptr = make_element_by_type<build_unit_close_button>(state, id);
			return ptr;
		} else if(name == "list") {
			auto ptr = make_element_by_type<units_build_listbox>(state, id);
			units_listbox = ptr.get();
			return ptr;
		} else if(name == "queue") {
			auto ptr = make_element_by_type<units_queue_listbox>(state, id);
			units_queue = ptr.get();
			return ptr;
		} else if(name == "external_scroll_slider_list") {
			auto ptr = make_element_by_type<element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "external_scroll_slider_queue") {
			auto ptr = make_element_by_type<element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name.substr(0, 12) == "unit_folder_") {
			auto ptr = make_element_by_type<unit_folder_button>(state, id);
			int32_t value = std::stoi(std::string(name.substr(12)));
			for(uint8_t i = 0; i < state.military_definitions.unit_base_definitions.size(); i++) {
				auto def = state.military_definitions.unit_base_definitions[dcon::unit_type_id(i)];
				if(def.icon == value) {
					ptr->unit_type = dcon::unit_type_id(i);
					if(def.is_land) {
						first_land_type = ptr->unit_type;
						army_elements.push_back(ptr.get());
					} else {
						first_naval_type = ptr->unit_type;
						navy_elements.push_back(ptr.get());
					}
				}
			}
			return ptr;
		} else {
			return nullptr;
		}
	}

	void set_army_visible(sys::state& state) {
		units_queue->is_navy = units_listbox->is_navy = false;
		for(auto element : army_elements)
			element->set_visible(state, true);
		units_listbox->impl_on_update(state);
		units_queue->impl_on_update(state);
	}

	void set_army_invisible(sys::state& state) {
		for(auto element : army_elements)
			element->set_visible(state, false);
	}

	void set_navy_visible(sys::state& state) {
		units_queue->is_navy = units_listbox->is_navy = true;
		for(auto element : navy_elements)
			element->set_visible(state, true);
		units_listbox->impl_on_update(state);
		units_queue->impl_on_update(state);
	}

	void set_navy_invisible(sys::state& state) {
		for(auto element : navy_elements)
			element->set_visible(state, false);
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::army_id>()) {
			unit_type = first_land_type;
			set_navy_invisible(state);
			set_army_visible(state);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::navy_id>()) {
			unit_type = first_naval_type;
			set_army_invisible(state);
			set_navy_visible(state);
			return message_result::consumed;
		}
		return message_result::unseen;
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<element_selection_wrapper<dcon::unit_type_id>>()) {
			unit_type = Cyto::any_cast<element_selection_wrapper<dcon::unit_type_id>>(payload).data;
			units_listbox->impl_on_update(state);
			units_queue->impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::unit_type_id>()) {
			payload.emplace<dcon::unit_type_id>(unit_type);
			return message_result::consumed;
		}
		return message_result::unseen;
	}

	std::vector<element_base*> army_elements;
	std::vector<element_base*> navy_elements;
	ui::units_build_listbox* units_listbox = nullptr;
	ui::units_queue_listbox* units_queue = nullptr;
};

} // namespace ui
