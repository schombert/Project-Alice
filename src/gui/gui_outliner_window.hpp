#pragma once

#include <variant>
#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"
#include "unit_tooltip.hpp"

namespace ui {

enum class outliner_filter : uint8_t {
	rebel_occupations,
	sieges,
	hostile_sieges,
	combat,
	naval_combat,
	armies,
	navies,
	factories,
	buildings,
	army_construction,
	navy_construction,
	gp_influence,
	national_focus,
	rally_points,
	count
};

struct outliner_rebel_occupation {
	dcon::province_id p;
};
struct outliner_hostile_siege {
	dcon::province_id p;
};
struct outliner_my_siege {
	dcon::province_id p;
};
struct outliner_rally_point {
	dcon::province_id p;
};

using outliner_data = std::variant< outliner_filter, dcon::army_id, dcon::navy_id, dcon::gp_relationship_id,
	dcon::state_building_construction_id, dcon::province_building_construction_id, dcon::province_land_construction_id,
	dcon::province_naval_construction_id, dcon::state_instance_id, outliner_rebel_occupation, outliner_hostile_siege, outliner_my_siege,
	dcon::land_battle_id, dcon::naval_battle_id, outliner_rally_point>;

class outliner_element_button : public shift_button_element_base {
public:
	bool visible = false;

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto content = retrieve< outliner_data>(state, parent);

	}
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve< outliner_data>(state, parent);
		visible = !std::holds_alternative<outliner_filter>(content);
		disabled = !visible;
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			shift_button_element_base::render(state, x, y);
	}
	void button_action(sys::state& state) noexcept override {
		if(!visible)
			return;

		auto content = retrieve< outliner_data>(state, parent);

		if(std::holds_alternative<dcon::army_id>(content)) {
			auto army = fatten(state.world, std::get<dcon::army_id>(content));
			state.selected_armies.clear();
			state.selected_navies.clear();
			state.select(army.id);
			state.map_state.center_map_on_province(state, army.get_location_from_army_location());
		} else if(std::holds_alternative<dcon::navy_id>(content)) {
			auto navy = fatten(state.world, std::get<dcon::navy_id>(content));
			state.selected_armies.clear();
			state.selected_navies.clear();
			state.select(navy.id);
			state.map_state.center_map_on_province(state, navy.get_location_from_navy_location());
		} else if(std::holds_alternative<dcon::gp_relationship_id>(content)) {
			auto grid = std::get<dcon::gp_relationship_id>(content);
			auto nid = state.world.gp_relationship_get_influence_target(grid);

			state.open_diplomacy(nid);
		} else if(std::holds_alternative<dcon::state_building_construction_id>(content)) {
			auto st_con = fatten(state.world, std::get<dcon::state_building_construction_id>(content));
			auto p = st_con.get_state().get_capital().id;

			static_cast<ui::province_view_window*>(state.ui_state.province_window)->set_active_province(state, p);
			state.map_state.center_map_on_province(state, p);
		} else if(std::holds_alternative<dcon::province_building_construction_id>(content)) {
			auto pbcid = std::get<dcon::province_building_construction_id>(content);
			auto p = state.world.province_building_construction_get_province(pbcid);

			static_cast<ui::province_view_window*>(state.ui_state.province_window)->set_active_province(state, p);
			state.map_state.center_map_on_province(state, p);
		} else if(std::holds_alternative<dcon::province_land_construction_id>(content)) {
			auto plcid = std::get<dcon::province_land_construction_id>(content);
			auto p = state.world.pop_get_province_from_pop_location(state.world.province_land_construction_get_pop(plcid));
			//static_cast<ui::province_view_window*>(state.ui_state.province_window)->set_active_province(state, p);
			//state.map_state.center_map_on_province(state, p);
		} else if(std::holds_alternative<dcon::province_naval_construction_id>(content)) {
			auto pncid = std::get<dcon::province_naval_construction_id>(content);
			auto p = state.world.province_naval_construction_get_province(pncid);
			//static_cast<ui::province_view_window*>(state.ui_state.province_window)->set_active_province(state, p);
			//state.map_state.center_map_on_province(state, p);
		} else if(std::holds_alternative<dcon::state_instance_id>(content)) {
			auto siid = std::get<dcon::state_instance_id>(content);
			auto fat_si = dcon::fatten(state.world, siid);
			auto p = fat_si.get_capital().id;
			static_cast<ui::province_view_window*>(state.ui_state.province_window)->set_active_province(state, p);
			state.map_state.center_map_on_province(state, p);
		} else if(std::holds_alternative<outliner_rebel_occupation>(content)) {
			auto p = std::get<outliner_rebel_occupation>(content).p;
			static_cast<ui::province_view_window*>(state.ui_state.province_window)->set_active_province(state, p);
			state.map_state.center_map_on_province(state, p);
		} else if(std::holds_alternative<outliner_hostile_siege>(content)) {
			auto p = std::get<outliner_hostile_siege>(content).p;
			static_cast<ui::province_view_window*>(state.ui_state.province_window)->set_active_province(state, p);
			state.map_state.center_map_on_province(state, p);
		} else if(std::holds_alternative<outliner_my_siege>(content)) {
			auto p = std::get<outliner_my_siege>(content).p;
			static_cast<ui::province_view_window*>(state.ui_state.province_window)->set_active_province(state, p);
			state.map_state.center_map_on_province(state, p);
		} else if(std::holds_alternative<dcon::land_battle_id>(content)) {
			auto b = std::get<dcon::land_battle_id>(content);
			auto p = state.world.land_battle_get_location_from_land_battle_location(b);
			//static_cast<ui::province_view_window*>(state.ui_state.province_window)->set_active_province(state, p);
			state.map_state.center_map_on_province(state, p);
		} else if(std::holds_alternative<dcon::naval_battle_id>(content)) {
			auto b = std::get<dcon::naval_battle_id>(content);
			auto p = state.world.naval_battle_get_location_from_naval_battle_location(b);
			//static_cast<ui::province_view_window*>(state.ui_state.province_window)->set_active_province(state, p);
			state.map_state.center_map_on_province(state, p);
		} else if(std::holds_alternative<outliner_rally_point>(content)) {
			auto p = std::get<outliner_rally_point>(content).p;
			static_cast<ui::province_view_window*>(state.ui_state.province_window)->set_active_province(state, p);
			state.map_state.center_map_on_province(state, p);
		}
	}
	void button_shift_action(sys::state& state) noexcept override {
		if(!visible)
			return;

		auto content = retrieve< outliner_data>(state, parent);
		if(std::holds_alternative<dcon::army_id>(content)) {
			auto army = fatten(state.world, std::get<dcon::army_id>(content));
			state.select(army.id);
			state.map_state.center_map_on_province(state, army.get_location_from_army_location());
		} else if(std::holds_alternative<dcon::navy_id>(content)) {
			auto navy = fatten(state.world, std::get<dcon::navy_id>(content));
			state.select(navy.id);
			state.map_state.center_map_on_province(state, navy.get_location_from_navy_location());
		} else {
			button_action(state);
		}
	}
};

class outliner_land_rally : public image_element_base {
public:
	bool visible = false;
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve< outliner_data>(state, parent);
		if(std::holds_alternative<outliner_rally_point>(content)) {
			auto p = std::get<outliner_rally_point>(content).p;
			visible = state.world.province_get_land_rally_point(p);
		} else {
			visible = false;
		}
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::render(state, x, y);
	}
};
class outliner_naval_rally : public image_element_base {
public:
	bool visible = false;
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve< outliner_data>(state, parent);
		if(std::holds_alternative<outliner_rally_point>(content)) {
			auto p = std::get<outliner_rally_point>(content).p;
			visible = state.world.province_get_naval_rally_point(p) && !state.world.province_get_land_rally_point(p);
		} else {
			visible = false;
		}
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::render(state, x, y);
	}
};
class outliner_moving_icon : public image_element_base {
public:
	bool visible = false;
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve< outliner_data>(state, parent);
		if(std::holds_alternative<dcon::army_id>(content)) {
			auto v = std::get<dcon::army_id>(content);
			visible = bool(state.world.army_get_arrival_time(v));
		} else if(std::holds_alternative<dcon::navy_id>(content)) {
			auto v = std::get<dcon::navy_id>(content);
			visible = bool(state.world.navy_get_arrival_time(v));
		} else {
			visible = false;
		}
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::render(state, x, y);
	}
};
class outliner_combat_icon : public image_element_base {
public:
	bool visible = false;
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve< outliner_data>(state, parent);
		if(std::holds_alternative<dcon::army_id>(content)) {
			auto v = std::get<dcon::army_id>(content);
			visible = bool(state.world.army_get_battle_from_army_battle_participation(v));
		} else if(std::holds_alternative<dcon::navy_id>(content)) {
			auto v = std::get<dcon::navy_id>(content);
			visible = bool(state.world.navy_get_battle_from_navy_battle_participation(v));
		} else if(std::holds_alternative<dcon::land_battle_id>(content)) {
			visible = true;
		} else if(std::holds_alternative<dcon::naval_battle_id>(content)) {
			visible = true;
		} else {
			visible = false;
		}
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::render(state, x, y);
	}
};
class outliner_header_bg : public image_element_base {
public:
	bool visible = false;
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve< outliner_data>(state, parent);
		visible = std::holds_alternative<outliner_filter>(content);
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::render(state, x, y);
	}
};
class outliner_header_text : public simple_text_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		base_data.data.text_common.font_handle = text::name_into_font_id(state, "garamond_14");
		simple_text_element_base::on_create(state);
	}
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve< outliner_data>(state, parent);
		if(std::holds_alternative<outliner_filter>(content)) {
			outliner_filter opt = std::get<outliner_filter>(content);

			switch(opt) {
			case outliner_filter::rebel_occupations:
				set_text(state, text::produce_simple_string(state, "ol_rebel_occupations"));
				break;
			case outliner_filter::sieges:
				set_text(state, text::produce_simple_string(state, "ol_sieges"));
				break;
			case outliner_filter::hostile_sieges:
				set_text(state, text::produce_simple_string(state, "ol_hostile_sieges"));
				break;
			case outliner_filter::combat:
				set_text(state, text::produce_simple_string(state, "ol_combat"));
				break;
			case outliner_filter::naval_combat:
				set_text(state, text::produce_simple_string(state, "ol_naval_combat"));
				break;
			case outliner_filter::armies:
				set_text(state, text::produce_simple_string(state, "ol_armies"));
				break;
			case outliner_filter::navies:
				set_text(state, text::produce_simple_string(state, "ol_navies"));
				break;
			case outliner_filter::factories:
				set_text(state, text::produce_simple_string(state, "ol_statebuilding_construction"));
				break;
			case outliner_filter::buildings:
				set_text(state, text::produce_simple_string(state, "ol_province_construction"));
				break;
			case outliner_filter::army_construction:
				set_text(state, text::produce_simple_string(state, "ol_army_construction"));
				break;
			case outliner_filter::navy_construction:
				set_text(state, text::produce_simple_string(state, "ol_navy_construciton"));
				break;
			case outliner_filter::gp_influence:
				set_text(state, text::produce_simple_string(state, "ol_gp_influence"));
				break;
			case outliner_filter::national_focus:
				set_text(state, text::produce_simple_string(state, "ol_view_natfocus_header"));
				break;
			case outliner_filter::rally_points:
				set_text(state, text::produce_simple_string(state, "ol_view_rallypoints_header"));
				break;
			default:
				set_text(state,  "???");
				break;
			}
		} else {
			set_text(state, "");
		}
	}
};

class outliner_entry_text : public color_text_element {
public:
	void on_create(sys::state& state) noexcept override {
		base_data.position.x = 16;
		base_data.size.x = 215;
		simple_text_element_base::on_create(state);
	}
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve< outliner_data>(state, parent);
		if(std::holds_alternative<outliner_filter>(content)) {
			set_text(state, "");
		} else if(std::holds_alternative<dcon::army_id>(content)) {
			auto army = fatten(state.world, std::get<dcon::army_id>(content));

			color = bool(army.get_battle_from_army_battle_participation()) ? text::text_color::red : text::text_color::white;
			unitamounts amounts = calc_amounts_from_army(state, army);

			text::substitution_map sub;
			auto tag_str = std::string("@") + nations::int_to_tag(army.get_controller_from_army_control().get_identity_from_identity_holder().get_identifying_int()) + "\x03";
			text::add_to_substitution_map(sub, text::variable_type::m, std::string_view{ tag_str });
			text::add_to_substitution_map(sub, text::variable_type::n, int64_t(amounts.type1));
			text::add_to_substitution_map(sub, text::variable_type::x, int64_t(amounts.type2));
			text::add_to_substitution_map(sub, text::variable_type::y, int64_t(amounts.type3));

			auto base_str = text::resolve_string_substitution(state, "unit_standing_text", sub);
			auto full_str = base_str + " (" + text::produce_simple_string(state, army.get_location_from_army_location().get_name()) + ")";
			set_text(state, full_str);
		} else if(std::holds_alternative<dcon::navy_id>(content)) {

			auto navy = fatten(state.world, std::get<dcon::navy_id>(content));

			color = bool(navy.get_battle_from_navy_battle_participation()) ? text::text_color::red : text::text_color::white;
			unitamounts amounts = calc_amounts_from_navy(state, navy);

			text::substitution_map sub;
			auto tag_str = std::string("@") + nations::int_to_tag(navy.get_controller_from_navy_control().get_identity_from_identity_holder().get_identifying_int()) + "\x04";
			text::add_to_substitution_map(sub, text::variable_type::m, std::string_view{ tag_str });
			text::add_to_substitution_map(sub, text::variable_type::n, int64_t(amounts.type1));
			text::add_to_substitution_map(sub, text::variable_type::x, int64_t(amounts.type2));
			text::add_to_substitution_map(sub, text::variable_type::y, int64_t(amounts.type3));

			auto base_str = text::resolve_string_substitution(state, "unit_standing_text", sub);
			auto full_str = base_str + " (" + text::produce_simple_string(state, navy.get_location_from_navy_location().get_name()) + ")";
			set_text(state, full_str);
		} else if(std::holds_alternative<dcon::gp_relationship_id>(content)) {
			auto grid = std::get<dcon::gp_relationship_id>(content);
			auto nid = state.world.gp_relationship_get_influence_target(grid);
			auto status = state.world.gp_relationship_get_status(grid);

			auto full_str = text::produce_simple_string(state, state.world.nation_get_name(nid)) + " (" + text::get_influence_level_name(state, status) + ")";

			color = text::text_color::white;
			set_text(state, full_str);
		} else if(std::holds_alternative<dcon::state_building_construction_id>(content)) {
			auto st_con = fatten(state.world, std::get<dcon::state_building_construction_id>(content));
			auto ftid = state.world.state_building_construction_get_type(st_con);

			float total = 0.0f;
			float purchased = 0.0f;
			auto& goods = state.world.factory_type_get_construction_costs(st_con.get_type());

			for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
				total += goods.commodity_amounts[i];
				purchased += st_con.get_purchased_goods().commodity_amounts[i];
			}
			auto progress = total > 0.0f ? purchased / total : 0.0f;

			auto full_str = text::produce_simple_string(state, state.world.factory_type_get_name(ftid)) + " (" + text::format_percentage(progress, 0) + ")";

			color = text::text_color::white;
			set_text(state, full_str);
		} else if(std::holds_alternative<dcon::province_building_construction_id>(content)) {
			auto pbcid = std::get<dcon::province_building_construction_id>(content);
			auto btid = state.world.province_building_construction_get_type(pbcid);
			auto name = economy::province_building_type_get_name(economy::province_building_type(btid));
			float progress = economy::province_building_construction(state, state.world.province_building_construction_get_province(pbcid), economy::province_building_type(btid)).progress;

			auto full_str = std::string(name) + " (" + text::format_percentage(progress, 0) + ")";

			color = text::text_color::white;
			set_text(state, full_str);
		} else if(std::holds_alternative<dcon::province_land_construction_id>(content)) {
			auto plcid = std::get<dcon::province_land_construction_id>(content);
			auto utid = state.world.province_land_construction_get_type(plcid);
			auto name = utid ? state.military_definitions.unit_base_definitions[utid].name : dcon::text_sequence_id{};
			float progress = economy::unit_construction_progress(state, plcid);

			auto full_str = text::produce_simple_string(state, name) + " (" + text::format_percentage(progress, 0) + ")";

			color = text::text_color::white;
			set_text(state, full_str);
		} else if(std::holds_alternative<dcon::province_naval_construction_id>(content)) {
			auto pncid = std::get<dcon::province_naval_construction_id>(content);
			auto utid = state.world.province_naval_construction_get_type(pncid);
			auto name = state.military_definitions.unit_base_definitions[utid].name;
			float progress = economy::unit_construction_progress(state, pncid);

			auto full_str = text::produce_simple_string(state, name) + " (" + text::format_percentage(progress, 0) + ")";
			color = text::text_color::white;
			set_text(state, full_str);
		} else if(std::holds_alternative<dcon::state_instance_id>(content)) {
			auto siid = std::get<dcon::state_instance_id>(content);
			auto fat_si = dcon::fatten(state.world, siid);
			auto fat_nf = dcon::fatten(state.world, siid).get_owner_focus();
			if(fat_nf.get_promotion_type()) {
				auto full_str = text::produce_simple_string(state, fat_nf.get_name()) + " (" + text::get_dynamic_state_name(state, siid) + ", "  + text::format_percentage(fat_si.get_demographics(demographics::to_key(state, fat_nf.get_promotion_type())) / fat_si.get_demographics(demographics::total)) + ")";
				color = text::text_color::white;
				set_text(state, full_str);
			} else {
				auto full_str = text::produce_simple_string(state, fat_nf.get_name()) + " (" + text::get_dynamic_state_name(state, siid) + ")";
				color = text::text_color::white;
				set_text(state, full_str);
			}
		} else if(std::holds_alternative<outliner_rebel_occupation>(content)) {
			auto p = std::get<outliner_rebel_occupation>(content).p;

			auto full_str = text::produce_simple_string(state, state.world.province_get_name(p));
			color = text::text_color::red;
			set_text(state, full_str);
		} else if(std::holds_alternative<outliner_hostile_siege>(content)) {
			auto p = std::get<outliner_hostile_siege>(content).p;

			auto full_str = text::produce_simple_string(state, state.world.province_get_name(p)) + " (" + text::format_percentage(state.world.province_get_siege_progress(p), 0) + ")";
			color = text::text_color::red;
			set_text(state, full_str);
		} else if(std::holds_alternative<outliner_my_siege>(content)) {
			auto p = std::get<outliner_my_siege>(content).p;

			auto full_str = text::produce_simple_string(state, state.world.province_get_name(p)) + " (" + text::format_percentage(state.world.province_get_siege_progress(p), 0) + ")";
			color = text::text_color::white;
			set_text(state, full_str);
		} else if(std::holds_alternative<dcon::land_battle_id>(content)) {
			auto p = std::get<dcon::land_battle_id>(content);

			auto full_str = text::produce_simple_string(state, state.world.province_get_name(state.world.land_battle_get_location_from_land_battle_location(p)));
			color = text::text_color::red;
			set_text(state, full_str);
		} else if(std::holds_alternative<dcon::naval_battle_id>(content)) {
			auto p = std::get<dcon::naval_battle_id>(content);

			auto full_str = text::produce_simple_string(state, state.world.province_get_name(state.world.naval_battle_get_location_from_naval_battle_location(p)));
			color = text::text_color::red;
			set_text(state, full_str);
		} else if(std::holds_alternative<outliner_rally_point>(content)) {
			auto p = std::get<outliner_rally_point>(content).p;

			auto full_str = text::produce_simple_string(state, state.world.province_get_name(p));
			color = text::text_color::white;
			set_text(state, full_str);
		}
	}
};

class outliner_element : public listbox2_row_element {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "outliner_header") {
			return make_element_by_type<outliner_header_bg>(state, id);
		} else if(name == "header_text") {
			return make_element_by_type<outliner_header_text>(state, id);
		} else if(name == "outliner_standard") {
			return make_element_by_type<outliner_element_button>(state, id);
		} else if(name == "combat_icon") {
			return make_element_by_type<outliner_combat_icon>(state, id);;
		} else if(name == "moving_icon") {
			return make_element_by_type<outliner_moving_icon>(state, id);
		} else if(name == "rally_land_icon") {
			return make_element_by_type<outliner_land_rally>(state, id);
		} else if(name == "rally_navy_icon") {
			return make_element_by_type<outliner_naval_rally>(state, id);
		} else if(name == "entry_text") {
			return make_element_by_type<outliner_entry_text>(state, id);
		} else if(name == "info_text") {
			return make_element_by_type<invisible_element>(state, id);
		} else {
			return nullptr;
		}
	}
};

class outliner_listbox : public listbox2_base<outliner_data> {
public:
	image_element_base* bottom_image = nullptr;

	std::unique_ptr<element_base> make_row(sys::state& state) noexcept override {
		return make_element_by_type<outliner_element>(state, "outliner_entry");
	}

	outliner_listbox() : listbox2_base<outliner_data>(true) { }

	bool get_filter(sys::state& state, outliner_filter filter) noexcept {
		if(parent) {
			Cyto::Any payload = filter;
			parent->impl_get(state, payload);
			return any_cast<bool>(payload);
		}
		return false;
	}

	void on_create(sys::state& state) noexcept override {
		listbox2_base<outliner_data>::on_create(state);

		auto ptr = make_element_by_type<image_element_base>(state, state.ui_state.defs_by_name.find("outliner_bottom")->second.definition);
		ptr->set_visible(state, true);
		bottom_image = ptr.get();
		add_child_to_back(std::move(ptr));
	}

	void on_update(sys::state& state) noexcept override {
		row_contents.clear();

		if(get_filter(state, outliner_filter::rebel_occupations)) {
			row_contents.push_back(outliner_filter::rebel_occupations);
			auto old_size = row_contents.size();
			for(auto op : state.world.nation_get_province_ownership(state.local_player_nation)) {
				if(!op.get_province().get_nation_from_province_control()) {
					row_contents.push_back(outliner_rebel_occupation{op.get_province().id });
				}
			}
			if(old_size == row_contents.size())
				row_contents.pop_back();
		}
		if(get_filter(state, outliner_filter::sieges)) {
			row_contents.push_back(outliner_filter::sieges);
			auto old_size = row_contents.size();

			std::vector<dcon::province_id> temp;
			for(auto ar : state.world.nation_get_army_control(state.local_player_nation)) {
				auto p = ar.get_army().get_location_from_army_location();
				if(p.get_siege_progress() > 0.0f) {
					if(std::find(temp.begin(), temp.end(), p.id) == temp.end()) {
						temp.push_back(p.id);
					}
				}
			}
			for(auto p : temp)
				row_contents.push_back(outliner_my_siege{ p });
			if(old_size == row_contents.size())
				row_contents.pop_back();
		}
		if(get_filter(state, outliner_filter::hostile_sieges)) {
			row_contents.push_back(outliner_filter::hostile_sieges);
			auto old_size = row_contents.size();
			for(auto op : state.world.nation_get_province_control(state.local_player_nation)) {
				if(op.get_province().get_siege_progress() > 0.0f) {
					row_contents.push_back(outliner_hostile_siege{ op.get_province().id });
				}
			}
			if(old_size == row_contents.size())
				row_contents.pop_back();
		}
		if(get_filter(state, outliner_filter::combat)) {
			row_contents.push_back(outliner_filter::combat);
			auto old_size = row_contents.size();

			std::vector<dcon::land_battle_id> temp;
			for(auto ar : state.world.nation_get_army_control(state.local_player_nation)) {
				auto p = ar.get_army().get_location_from_army_location();
				if(auto b = ar.get_army().get_battle_from_army_battle_participation(); b) {
					if(std::find(temp.begin(), temp.end(), b.id) == temp.end()) {
						temp.push_back(b.id);
					}
				}
			}
			for(auto p : temp)
				row_contents.push_back( p );
			if(old_size == row_contents.size())
				row_contents.pop_back();
		}
		if(get_filter(state, outliner_filter::naval_combat)) {
			row_contents.push_back(outliner_filter::naval_combat);
			auto old_size = row_contents.size();

			std::vector<dcon::naval_battle_id> temp;
			for(auto n : state.world.nation_get_navy_control(state.local_player_nation)) {
				auto p = n.get_navy().get_location_from_navy_location();
				if(auto b = n.get_navy().get_battle_from_navy_battle_participation(); b) {
					if(std::find(temp.begin(), temp.end(), b.id) == temp.end()) {
						temp.push_back(b.id);
					}
				}
			}
			for(auto p : temp)
				row_contents.push_back( p );
			if(old_size == row_contents.size())
				row_contents.pop_back();
		}
		if(get_filter(state, outliner_filter::armies)) {
			row_contents.push_back(outliner_filter::armies);
			auto old_size = row_contents.size();
			state.world.nation_for_each_army_control_as_controller(state.local_player_nation, [&](dcon::army_control_id acid) {
				auto aid = state.world.army_control_get_army(acid);
				row_contents.push_back(aid);
			});
			if(old_size == row_contents.size())
				row_contents.pop_back();
		}
		if(get_filter(state, outliner_filter::navies)) {
			row_contents.push_back(outliner_filter::navies);
			auto old_size = row_contents.size();
			state.world.nation_for_each_navy_control_as_controller(state.local_player_nation, [&](dcon::navy_control_id ncid) {
				auto nid = state.world.navy_control_get_navy(ncid);
				row_contents.push_back(nid);
			});
			if(old_size == row_contents.size())
				row_contents.pop_back();
		}
		if(get_filter(state, outliner_filter::factories)) {
			row_contents.push_back(outliner_filter::factories);
			auto old_size = row_contents.size();
			state.world.nation_for_each_state_building_construction(state.local_player_nation,
					[&](dcon::state_building_construction_id sbcid) { row_contents.push_back(sbcid); });
			if(old_size == row_contents.size())
				row_contents.pop_back();
		}
		if(get_filter(state, outliner_filter::buildings)) {
			row_contents.push_back(outliner_filter::buildings);
			auto old_size = row_contents.size();
			state.world.nation_for_each_province_building_construction(state.local_player_nation,
					[&](dcon::province_building_construction_id pbcid) { row_contents.push_back(pbcid); });
			if(old_size == row_contents.size())
				row_contents.pop_back();
		}
		if(get_filter(state, outliner_filter::army_construction)) {
			row_contents.push_back(outliner_filter::army_construction);
			auto old_size = row_contents.size();
			state.world.nation_for_each_province_land_construction(state.local_player_nation,
					[&](dcon::province_land_construction_id plcid) { row_contents.push_back(plcid); });
			if(old_size == row_contents.size())
				row_contents.pop_back();
		}
		if(get_filter(state, outliner_filter::navy_construction)) {
			row_contents.push_back(outliner_filter::navy_construction);
			auto old_size = row_contents.size();
			state.world.nation_for_each_province_naval_construction(state.local_player_nation,
					[&](dcon::province_naval_construction_id pncid) { row_contents.push_back(pncid); });
			if(old_size == row_contents.size())
				row_contents.pop_back();
		}
		if(get_filter(state, outliner_filter::gp_influence)) {
			row_contents.push_back(outliner_filter::gp_influence);
			auto old_size = row_contents.size();
			state.world.nation_for_each_gp_relationship_as_great_power(state.local_player_nation,
					[&](dcon::gp_relationship_id grid) { row_contents.push_back(grid); });
			if(old_size == row_contents.size())
				row_contents.pop_back();
		}
		if(get_filter(state, outliner_filter::national_focus)) {
			row_contents.push_back(outliner_filter::national_focus);
			auto old_size = row_contents.size();
			state.world.nation_for_each_state_ownership(state.local_player_nation, [&](dcon::state_ownership_id soid) {
				auto fat = dcon::fatten(state.world, soid).get_state();
				if(fat.get_owner_focus().is_valid()) {
					row_contents.push_back(fat.id);
				}
			});
			if(old_size == row_contents.size())
				row_contents.pop_back();
		}
		if(get_filter(state, outliner_filter::rally_points)) {
			row_contents.push_back(outliner_filter::rally_points);
			auto old_size = row_contents.size();

			for(auto op : state.world.nation_get_province_ownership(state.local_player_nation)) {
				if(op.get_province().get_land_rally_point() || op.get_province().get_naval_rally_point()) {
					row_contents.push_back(outliner_rally_point{ op.get_province().id });
				}
			}
			if(old_size == row_contents.size())
				row_contents.pop_back();
		}

		auto rsz = row_windows[0]->base_data.size.y + row_windows[0]->base_data.position.y;
		auto max_size = int32_t(rsz * row_contents.size());
		auto space = (state.ui_state.root->base_data.size.y - 330) - ui::get_absolute_location(*this).y;
		auto max_rows = space / rsz;

		if(max_size < space) {
			if(visible_row_count != int32_t(row_contents.size())) {
				resize(state, max_size);
			}
		} else {
			if(visible_row_count != max_rows) {
				resize(state, max_rows * rsz);
			}
		}

		listbox2_base<outliner_data>::on_update(state);

		bottom_image->base_data.position.y = int16_t(base_data.size.y);
	}
};

class outliner_minmax_button : public button_element_base {
public:
};

template<outliner_filter Filter>
class outliner_filter_checkbox : public checkbox_button {
	static std::string_view get_filter_text_key(outliner_filter f) noexcept {
		switch(f) {
		case outliner_filter::rebel_occupations:
			return "ol_view_rebel_occupations";
		case outliner_filter::sieges:
			return "remove_ol_view_sieges";
		case outliner_filter::hostile_sieges:
			return "remove_ol_view_hostile_sieges";
		case outliner_filter::combat:
			return "ol_view_combat";
		case outliner_filter::naval_combat:
			return "ol_view_navalcombat";
		case outliner_filter::armies:
			return "ol_view_armies";
		case outliner_filter::navies:
			return "ol_view_navies";
		case outliner_filter::factories:
			return "ol_view_factories";
		case outliner_filter::buildings:
			return "ol_view_buildings";
		case outliner_filter::army_construction:
			return "ol_view_army_construction";
		case outliner_filter::navy_construction:
			return "ol_view_navy_construction";
		case outliner_filter::gp_influence:
			return "ol_view_gp_influence";
		case outliner_filter::national_focus:
			return "ol_view_natfocus";
		case outliner_filter::rally_points:
			return "ol_view_rallypoints";
		default:
			return "???";
		}
	}

public:
	bool is_active(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = Filter;
			parent->impl_get(state, payload);
			return any_cast<bool>(payload);
		}
		return false;
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = Filter;
			parent->impl_set(state, payload);
			state.ui_state.outliner_window->impl_on_update(state);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto name = get_filter_text_key(Filter);
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view(name));
		text::close_layout_box(contents, box);
	}
};

class outliner_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(state.ui_state.outliner_window)
			frame = state.ui_state.outliner_window->is_visible() ? 1 : 0;
	}

	void button_action(sys::state& state) noexcept override {
		if(state.ui_state.outliner_window) {
			state.ui_state.outliner_window->set_visible(state, !state.ui_state.outliner_window->is_visible());
			on_update(state);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		if(!state.ui_state.outliner_window->is_visible()) {
			text::localised_format_box(state, contents, box, std::string_view("topbar_open_outliner"));
		} else {
			text::localised_format_box(state, contents, box, std::string_view("topbar_close_outliner"));
		}
		text::close_layout_box(contents, box);
	}
};

class outliner_window : public window_element_base {
	outliner_listbox* listbox = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "outliner_top") {
			return make_element_by_type<outliner_minmax_button>(state, id);
		} else if(name == "outliner_list") {
			auto ptr = make_element_by_type<outliner_listbox>(state, id);
			listbox = ptr.get();
			return ptr;
		} else if(name == "outliner_view_rebel_occupations") {
			return make_element_by_type<outliner_filter_checkbox<outliner_filter::rebel_occupations>>(state, id);
		} else if(name == "outliner_view_sieges") {
			return make_element_by_type<outliner_filter_checkbox<outliner_filter::sieges>>(state, id);
		} else if(name == "outliner_view_hostile_sieges") {
			return make_element_by_type<outliner_filter_checkbox<outliner_filter::hostile_sieges>>(state, id);
		} else if(name == "outliner_view_combat") {
			return make_element_by_type<outliner_filter_checkbox<outliner_filter::combat>>(state, id);
		} else if(name == "outliner_view_navalcombat") {
			return make_element_by_type<outliner_filter_checkbox<outliner_filter::naval_combat>>(state, id);
		} else if(name == "outliner_view_armies") {
			return make_element_by_type<outliner_filter_checkbox<outliner_filter::armies>>(state, id);
		} else if(name == "outliner_view_navies") {
			return make_element_by_type<outliner_filter_checkbox<outliner_filter::navies>>(state, id);
		} else if(name == "outliner_view_factories") {
			return make_element_by_type<outliner_filter_checkbox<outliner_filter::factories>>(state, id);
		} else if(name == "outliner_view_buildings") {
			return make_element_by_type<outliner_filter_checkbox<outliner_filter::buildings>>(state, id);
		} else if(name == "outliner_view_army_construction") {
			return make_element_by_type<outliner_filter_checkbox<outliner_filter::army_construction>>(state, id);
		} else if(name == "outliner_view_navy_construction") {
			return make_element_by_type<outliner_filter_checkbox<outliner_filter::navy_construction>>(state, id);
		} else if(name == "outliner_view_gp_influence") {
			return make_element_by_type<outliner_filter_checkbox<outliner_filter::gp_influence>>(state, id);
		} else if(name == "outliner_view_natfocus") {
			return make_element_by_type<outliner_filter_checkbox<outliner_filter::national_focus>>(state, id);
		} else if(name == "outliner_view_rallypoints") {
			return make_element_by_type<outliner_filter_checkbox<outliner_filter::rally_points>>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<outliner_filter>()) {
			auto filter = any_cast<outliner_filter>(payload);

			state.user_settings.outliner_views[uint8_t(filter)] = !state.user_settings.outliner_views[uint8_t(filter)];
			state.save_user_settings();

			listbox->on_update(state);
			return message_result::consumed;
		}
		return message_result::unseen;
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<outliner_filter>()) {
			auto filter = any_cast<outliner_filter>(payload);
			payload.emplace<bool>(state.user_settings.outliner_views[uint8_t(filter)]);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};
} // namespace ui
