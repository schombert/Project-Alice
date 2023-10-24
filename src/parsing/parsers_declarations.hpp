#pragma once

#include <stdint.h>
#include <optional>
#include <string_view>
#include <string>

#include "constants.hpp"
#include "parsers.hpp"
#include "unordered_dense.h"
#include "gui_graphics.hpp"
#include "modifiers.hpp"
#include "culture.hpp"
#include "date_interface.hpp"
#include "script_constants.hpp"
#include "container_types.hpp"
#include "military.hpp"
#include "nations.hpp"

namespace parsers {

//
// structures and functions for parsing .gfx files
//
std::string lowercase_str(std::string_view sv);

struct building_gfx_context {
	sys::state& full_state;
	ui::definitions& ui_defs;
	ankerl::unordered_dense::map<std::string, dcon::gfx_object_id> map_of_names;
	ankerl::unordered_dense::map<std::string, dcon::texture_id> map_of_texture_names;
	bool on_second_pair_y = false;
	building_gfx_context(sys::state& full_state, ui::definitions& ui_defs) : full_state(full_state), ui_defs(ui_defs) { }
};

struct gfx_xy_pair {
	int32_t x = 0;
	int32_t y = 0;

	void free_value(int32_t v, error_handler& err, int32_t line, building_gfx_context& context) {
		if(context.on_second_pair_y) {
			if(y != 0) {
				err.accumulated_errors += "More than 2 elements for pair " + err.file_name + " line " + std::to_string(line) + "\n"; 
			}
			y = v;
		} else {
			x = v;
			context.on_second_pair_y = true;
		}
	}
	void finish(building_gfx_context& context) {
		context.on_second_pair_y = false;
	}
};

struct gfx_object {
	std::optional<int32_t> size;
	std::optional<gfx_xy_pair> size_obj;
	std::optional<gfx_xy_pair> bordersize;
	std::string_view name;
	std::string_view primary_texture;
	std::string_view secondary_texture;

	int32_t noofframes = 1;

	bool horizontal = true;
	bool allwaystransparent = false;
	bool flipv = false;
	bool clicksound_set = false;
	bool transparencecheck = false;

	void clicksound(parsers::association_type, std::string_view, parsers::error_handler& err, int32_t line,
			building_gfx_context& context) {
		clicksound_set = true;
	}

	void finish(building_gfx_context& context) { }
};

struct gfx_object_outer {
	void spritetype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context& context);
	void corneredtilespritetype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context& context);
	void maskedshieldtype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context& context);
	void textspritetype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context& context);
	void tilespritetype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context& context);
	void progressbartype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context& context);
	void barcharttype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context& context);
	void piecharttype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context& context);
	void linecharttype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context& context);

	void finish(building_gfx_context const& context) { }
};

void gfx_add_obj(parsers::token_generator& gen, parsers::error_handler& err, building_gfx_context& context);

struct gfx_files {
	void finish(building_gfx_context& context) { }
};

//
// structures and functions for parsing .gui files
//

struct gui_element_common {
	ui::element_data target;

	gui_element_common() { }
	void size(gfx_xy_pair const& pr, error_handler& err, int32_t line, building_gfx_context& context);
	void position(gfx_xy_pair const& pr, error_handler& err, int32_t line, building_gfx_context& context);
	void orientation(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context);
	void name(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context);
	void rotation(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context);
	void maxwidth(association_type, int32_t v, error_handler& err, int32_t line, building_gfx_context& context);
	void maxheight(association_type, int32_t v, error_handler& err, int32_t line, building_gfx_context& context);
	void finish(building_gfx_context& context) { }
};

struct button : public gui_element_common {
	button();
	void spritetype(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context);
	void shortcut(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context);
	void buttontext(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context);
	void buttonfont(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context);
	void format(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context);
	void clicksound(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context);
};

struct image : public gui_element_common {
	image();
	void frame(association_type, uint32_t v, error_handler& err, int32_t line, building_gfx_context& context);
	void spritetype(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context);
	void scale(association_type, float v, error_handler& err, int32_t line, building_gfx_context& context);
};

struct textbox : public gui_element_common {
	textbox();
	void bordersize(gfx_xy_pair const& pr, error_handler& err, int32_t line, building_gfx_context& context);
	void fixedsize(association_type, bool v, error_handler& err, int32_t line, building_gfx_context& context);
	void font(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context);
	void format(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context);
	void text(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context);
	void texturefile(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context);
};

struct listbox : public gui_element_common {
	listbox();
	void background(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context);
	void bordersize(gfx_xy_pair const& pr, error_handler& err, int32_t line, building_gfx_context& context);
	void offset(gfx_xy_pair const& pr, error_handler& err, int32_t line, building_gfx_context& context);
	void spacing(association_type, uint32_t v, error_handler& err, int32_t line, building_gfx_context& context);
};

struct overlapping : public gui_element_common {
	overlapping();
	void format(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context);
	void spacing(association_type, float v, error_handler& err, int32_t line, building_gfx_context& context);
};

struct scrollbar : public gui_element_common {
	std::vector<ui::element_data> children;
	std::string_view leftbutton_;
	std::string_view rightbutton_;
	std::string_view rangelimitmaxicon_;
	std::string_view rangelimitminicon_;
	std::string_view track_;
	std::string_view slider_;

	scrollbar();
	void horizontal(association_type, bool v, error_handler& err, int32_t line, building_gfx_context& context);
	void leftbutton(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context);
	void rightbutton(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context);
	void lockable(association_type, bool v, error_handler& err, int32_t line, building_gfx_context& context);
	void maxvalue(association_type, uint32_t v, error_handler& err, int32_t line, building_gfx_context& context);
	void rangelimitmaxicon(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context);
	void rangelimitminicon(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context);
	void slider(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context);
	void track(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context);
	void stepsize(association_type, std::string_view txt, error_handler& err, int32_t line, building_gfx_context& context);
	void userangelimit(association_type, bool v, error_handler& err, int32_t line, building_gfx_context& context);
	void bordersize(gfx_xy_pair const& pr, error_handler& err, int32_t line, building_gfx_context& context);
	void guibuttontype(button const& v, error_handler& err, int32_t line, building_gfx_context& context);
	void icontype(image const& v, error_handler& err, int32_t line, building_gfx_context& context);
	void finish(building_gfx_context& context);
};

struct window : public gui_element_common {
	std::vector<ui::element_data> children;
	window();
	void fullscreen(association_type, bool v, error_handler& err, int32_t line, building_gfx_context& context);
	void moveable(association_type, bool v, error_handler& err, int32_t line, building_gfx_context& context);
	void guibuttontype(button const& v, error_handler& err, int32_t line, building_gfx_context& context);
	void icontype(image const& v, error_handler& err, int32_t line, building_gfx_context& context);
	void eu3dialogtype(window const& v, error_handler& err, int32_t line, building_gfx_context& context);
	void instanttextboxtype(textbox const& v, error_handler& err, int32_t line, building_gfx_context& context);
	void listboxtype(listbox const& v, error_handler& err, int32_t line, building_gfx_context& context);
	void positiontype(gui_element_common const& v, error_handler& err, int32_t line, building_gfx_context& context);
	void scrollbartype(scrollbar const& v, error_handler& err, int32_t line, building_gfx_context& context);
	void windowtype(window const& v, error_handler& err, int32_t line, building_gfx_context& context);
	void checkboxtype(button const& v, error_handler& err, int32_t line, building_gfx_context& context);
	void shieldtype(image const& v, error_handler& err, int32_t line, building_gfx_context& context);
	void overlappingelementsboxtype(overlapping const& v, error_handler& err, int32_t line, building_gfx_context& context);
	void editboxtype(textbox const& v, error_handler& err, int32_t line, building_gfx_context& context);
	void textboxtype(textbox const& v, error_handler& err, int32_t line, building_gfx_context& context);
	void finish(building_gfx_context& context);
};

struct guitypes {
	void finish(building_gfx_context& context) { }
	void guibuttontype(button const& v, error_handler& err, int32_t line, building_gfx_context& context);
	void icontype(image const& v, error_handler& err, int32_t line, building_gfx_context& context);
	void eu3dialogtype(window const& v, error_handler& err, int32_t line, building_gfx_context& context);
	void instanttextboxtype(textbox const& v, error_handler& err, int32_t line, building_gfx_context& context);
	void listboxtype(listbox const& v, error_handler& err, int32_t line, building_gfx_context& context);
	void positiontype(gui_element_common const& v, error_handler& err, int32_t line, building_gfx_context& context);
	void scrollbartype(scrollbar const& v, error_handler& err, int32_t line, building_gfx_context& context);
	void windowtype(window const& v, error_handler& err, int32_t line, building_gfx_context& context);
	void checkboxtype(button const& v, error_handler& err, int32_t line, building_gfx_context& context);
	void shieldtype(image const& v, error_handler& err, int32_t line, building_gfx_context& context);
	void overlappingelementsboxtype(overlapping const& v, error_handler& err, int32_t line, building_gfx_context& context);
	void editboxtype(textbox const& v, error_handler& err, int32_t line, building_gfx_context& context);
	void textboxtype(textbox const& v, error_handler& err, int32_t line, building_gfx_context& context);
};
struct gui_files {
	void finish(building_gfx_context& context) { }
};

struct pending_ideology_content {
	token_generator generator_state;
	dcon::ideology_id id;
};
struct pending_option_content {
	token_generator generator_state;
	dcon::issue_option_id id;
};
struct pending_roption_content {
	token_generator generator_state;
	dcon::reform_option_id id;
};
struct pending_cb_content {
	token_generator generator_state;
	dcon::cb_type_id id;
};
struct pending_crime_content {
	token_generator generator_state;
	dcon::crime_id id;
};
struct pending_triggered_modifier_content {
	token_generator generator_state;
	uint32_t index;
};
struct pending_rebel_type_content {
	token_generator generator_state;
	dcon::rebel_type_id id;
};

struct province_data {
	int32_t id = 0; // original
	bool is_sea = false;
};
struct terrain_type {
	dcon::modifier_id id;
	uint32_t color = 0;
};
struct pending_tech_content {
	token_generator generator_state;
	dcon::technology_id id;
};
struct pending_invention_content {
	token_generator generator_state;
	dcon::invention_id id;
};
struct pending_nat_event {
	std::string original_file;
	dcon::national_event_id id;
	trigger::slot_contents main_slot;
	trigger::slot_contents this_slot;
	trigger::slot_contents from_slot;
	token_generator generator_state;
	bool text_assigned = false;
	bool processed = false;
	bool just_in_case_placeholder = false;

	pending_nat_event() = default;
	pending_nat_event(dcon::national_event_id id, trigger::slot_contents main_slot, trigger::slot_contents this_slot,
			trigger::slot_contents from_slot)
			: id(id), main_slot(main_slot), this_slot(this_slot), from_slot(from_slot), just_in_case_placeholder(false) { }
	pending_nat_event(dcon::national_event_id id, trigger::slot_contents main_slot, trigger::slot_contents this_slot,
			trigger::slot_contents from_slot, token_generator const& generator_state)
			: id(id), main_slot(main_slot), this_slot(this_slot), from_slot(from_slot), generator_state(generator_state),
				text_assigned(true), just_in_case_placeholder(false){ }
	pending_nat_event(std::string const& original_file, dcon::national_event_id id, trigger::slot_contents main_slot, trigger::slot_contents this_slot,
			trigger::slot_contents from_slot, token_generator const& generator_state, bool just_in_case_placeholder)
		: original_file(original_file), id(id), main_slot(main_slot), this_slot(this_slot), from_slot(from_slot), generator_state(generator_state),
		text_assigned(true), just_in_case_placeholder(just_in_case_placeholder) { }
};
struct pending_prov_event {
	std::string original_file;
	dcon::provincial_event_id id;
	trigger::slot_contents main_slot;
	trigger::slot_contents this_slot;
	trigger::slot_contents from_slot;
	token_generator generator_state;
	bool text_assigned = false;
	bool processed = false;
	bool just_in_case_placeholder = false;

	pending_prov_event() = default;
	pending_prov_event(dcon::provincial_event_id id, trigger::slot_contents main_slot, trigger::slot_contents this_slot,
			trigger::slot_contents from_slot)
			: id(id), main_slot(main_slot), this_slot(this_slot), from_slot(from_slot), just_in_case_placeholder(false) { }
	pending_prov_event(dcon::provincial_event_id id, trigger::slot_contents main_slot, trigger::slot_contents this_slot,
			trigger::slot_contents from_slot, token_generator const& generator_state)
			: id(id), main_slot(main_slot), this_slot(this_slot), from_slot(from_slot), generator_state(generator_state),
				text_assigned(true), just_in_case_placeholder(false) { }
	pending_prov_event(std::string const& original_file, dcon::provincial_event_id id, trigger::slot_contents main_slot, trigger::slot_contents this_slot,
			trigger::slot_contents from_slot, token_generator const& generator_state, bool just_in_case_placeholder)
		: original_file(original_file), id(id), main_slot(main_slot), this_slot(this_slot), from_slot(from_slot), generator_state(generator_state),
		text_assigned(true), just_in_case_placeholder(just_in_case_placeholder) { }
};
struct scenario_building_context {
	building_gfx_context gfx_context;

	sys::state& state;

	ankerl::unordered_dense::map<uint32_t, dcon::national_identity_id> map_of_ident_names;
	tagged_vector<std::string, dcon::national_identity_id> file_names_for_idents;

	ankerl::unordered_dense::map<std::string, dcon::religion_id> map_of_religion_names;
	ankerl::unordered_dense::map<std::string, dcon::culture_id> map_of_culture_names;
	ankerl::unordered_dense::map<std::string, dcon::culture_group_id> map_of_culture_group_names;
	ankerl::unordered_dense::map<std::string, dcon::commodity_id> map_of_commodity_names;
	ankerl::unordered_dense::map<std::string, dcon::factory_type_id> map_of_production_types;
	ankerl::unordered_dense::map<std::string, dcon::factory_type_id> map_of_factory_names;
	ankerl::unordered_dense::map<std::string, pending_ideology_content> map_of_ideologies;
	ankerl::unordered_dense::map<std::string, dcon::ideology_group_id> map_of_ideology_groups;
	ankerl::unordered_dense::map<std::string, pending_option_content> map_of_ioptions;
	ankerl::unordered_dense::map<std::string, pending_roption_content> map_of_roptions;
	ankerl::unordered_dense::map<std::string, dcon::issue_id> map_of_iissues;
	ankerl::unordered_dense::map<std::string, dcon::reform_id> map_of_reforms;
	ankerl::unordered_dense::map<std::string, dcon::government_type_id> map_of_governments;
	ankerl::unordered_dense::map<std::string, pending_cb_content> map_of_cb_types;
	ankerl::unordered_dense::map<std::string, dcon::leader_trait_id> map_of_leader_traits;
	ankerl::unordered_dense::map<std::string, pending_crime_content> map_of_crimes;
	std::vector<pending_triggered_modifier_content> set_of_triggered_modifiers;
	ankerl::unordered_dense::map<std::string, dcon::modifier_id> map_of_modifiers;
	ankerl::unordered_dense::map<std::string, dcon::pop_type_id> map_of_poptypes;
	ankerl::unordered_dense::map<std::string, pending_rebel_type_content> map_of_rebeltypes;
	ankerl::unordered_dense::map<std::string, terrain_type> map_of_terrain_types;
	ankerl::unordered_dense::map<std::string, int32_t> map_of_tech_folders;
	ankerl::unordered_dense::map<std::string, pending_tech_content> map_of_technologies;
	ankerl::unordered_dense::map<std::string, pending_invention_content> map_of_inventions;
	ankerl::unordered_dense::map<std::string, dcon::unit_type_id> map_of_unit_types;
	ankerl::unordered_dense::map<std::string, dcon::national_variable_id> map_of_national_variables;
	ankerl::unordered_dense::map<std::string, dcon::national_flag_id> map_of_national_flags;
	ankerl::unordered_dense::map<std::string, dcon::global_flag_id> map_of_global_flags;
	ankerl::unordered_dense::map<std::string, dcon::state_definition_id> map_of_state_names;
	ankerl::unordered_dense::map<std::string, dcon::region_id> map_of_region_names;
	ankerl::unordered_dense::map<int32_t, pending_nat_event> map_of_national_events;
	ankerl::unordered_dense::map<int32_t, pending_prov_event> map_of_provincial_events;
	ankerl::unordered_dense::map<std::string, dcon::leader_images_id> map_of_leader_graphics;

	tagged_vector<province_data, dcon::province_id> prov_id_to_original_id_map;
	std::vector<dcon::province_id> original_id_to_prov_id_map;

	ankerl::unordered_dense::map<uint32_t, dcon::province_id> map_color_to_province_id;

	std::optional<simple_fs::file> ideologies_file;
	std::optional<simple_fs::file> issues_file;
	std::optional<simple_fs::file> cb_types_file;
	std::optional<simple_fs::file> crimes_file;
	std::optional<simple_fs::file> triggered_modifiers_file;
	std::optional<simple_fs::file> rebel_types_file;
	std::vector<simple_fs::file> tech_and_invention_files;

	dcon::text_key noimage;

	dcon::modifier_id modifier_by_terrain_index[64] = {}; // these are the given mappings from the raw palette index to terrain type
	uint32_t color_by_terrain_index[64] = {
			0}; // these are the (packed) colors given for the terrain type modifier at the given palette index
	dcon::modifier_id ocean_terrain;

	scenario_building_context(sys::state& state);

	dcon::national_variable_id get_national_variable(std::string const& name);
	dcon::national_flag_id get_national_flag(std::string const& name);
	dcon::global_flag_id get_global_flag(std::string const& name);

	int32_t number_of_commodities_seen = 0;
	int32_t number_of_national_values_seen = 0;
};

struct national_identity_file {
	void any_value(std::string_view tag, association_type, std::string_view txt, error_handler& err, int32_t line,
			scenario_building_context& context);
	void finish(scenario_building_context& context) { }
};

struct color_from_3f {
	uint32_t value = 0;
	template<typename T>
	void free_value(float v, error_handler& err, int32_t line, T& context) {
		value = (value >> 8) | (uint32_t(v * 255.0f) << 16);
	}
	template<typename T>
	void finish(T& context) { }
};

struct religion_context {
	dcon::religion_id id;
	scenario_building_context& outer_context;
};
struct religion_def {
	void icon(association_type, int32_t v, error_handler& err, int32_t line, religion_context& context);
	void color(color_from_3f v, error_handler& err, int32_t line, religion_context& context);
	void pagan(association_type, bool v, error_handler& err, int32_t line, religion_context& context);
	void finish(religion_context& context) { }
};

void make_religion(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);

struct religion_group {
	void finish(scenario_building_context& context) { }
};

struct religion_file {
	void any_group(std::string_view name, religion_group, error_handler& err, int32_t line, scenario_building_context& context) { }
	void finish(scenario_building_context& context) { }
};

struct color_from_3i {
	uint32_t value = 0;
	template<typename T>
	void free_value(int32_t v, error_handler& err, int32_t line, T& context) {
		value = (value >> 8) | (uint32_t(v & 0xFF) << 16);
	}
	template<typename T>
	void finish(T& context) { }
};

struct culture_group_context {
	dcon::culture_group_id id;
	scenario_building_context& outer_context;
};
struct culture_context {
	dcon::culture_id id;
	scenario_building_context& outer_context;
};
struct names_context {
	dcon::culture_id id;
	bool first_names = false;
	scenario_building_context& outer_context;
};

void make_culture_group(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);
void make_culture(std::string_view name, token_generator& gen, error_handler& err, culture_group_context& context);
void make_fn_list(token_generator& gen, error_handler& err, culture_context& context);
void make_ln_list(token_generator& gen, error_handler& err, culture_context& context);

struct names_list {
	void free_value(std::string_view text, error_handler& err, int32_t line, names_context& context);
	void finish(names_context& context) { }
};

struct culture {
	void color(color_from_3i v, error_handler& err, int32_t line, culture_context& context);
	void radicalism(association_type, int32_t v, error_handler& err, int32_t line, culture_context& context);
	void finish(culture_context& context) { }
};

struct culture_group {
	void leader(association_type, std::string_view name, error_handler& err, int32_t line, culture_group_context& context);
	void is_overseas(association_type, bool v, error_handler& err, int32_t line, culture_group_context& context);
	void union_tag(association_type, uint32_t v, error_handler& err, int32_t line, culture_group_context& context);
	void finish(culture_group_context& context) { }
};

struct culture_file {
	void finish(scenario_building_context& context) { }
};

struct good_group_context {
	sys::commodity_group group = sys::commodity_group::consumer_goods;
	scenario_building_context& outer_context;
};
struct good_context {
	dcon::commodity_id id;
	scenario_building_context& outer_context;
};

struct good {
	void money(association_type, bool v, error_handler& err, int32_t line, good_context& context);
	void color(color_from_3i v, error_handler& err, int32_t line, good_context& context);
	void cost(association_type, float v, error_handler& err, int32_t line, good_context& context);
	void available_from_start(association_type, bool b, error_handler& err, int32_t line, good_context& context);
	void overseas_penalty(association_type, bool b, error_handler& err, int32_t line, good_context& context);

	void finish(good_context& context);
};

void make_good(std::string_view name, token_generator& gen, error_handler& err, good_group_context& context);
void make_goods_group(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);

struct goods_group {
	void finish(good_group_context& context) { }
};

struct goods_file {
	void finish(scenario_building_context& context) { }
};

#define MOD_PROV_FUNCTION(X)                                                                                                     \
	template<typename T>                                                                                                           \
	void X(association_type, float v, error_handler& err, int32_t line, T& context) {                                              \
		if(next_to_add_p >= sys::provincial_modifier_definition::modifier_definition_size) {                                         \
			err.accumulated_errors += "Too many modifier values; " + err.file_name + " line " + std::to_string(line) + "\n";           \
		} else {                                                                                                                     \
			constructed_definition_p.offsets[next_to_add_p] = sys::provincial_mod_offsets::X;                                          \
			constructed_definition_p.values[next_to_add_p] = v;                                                                        \
			++next_to_add_p;                                                                                                           \
		}                                                                                                                            \
	}
#define MOD_NAT_FUNCTION(X)                                                                                                      \
	template<typename T>                                                                                                           \
	void X(association_type, float v, error_handler& err, int32_t line, T& context) {                                              \
		if(next_to_add_n >= sys::national_modifier_definition::modifier_definition_size) {                                           \
			err.accumulated_errors += "Too many modifier values; " + err.file_name + " line " + std::to_string(line) + "\n";           \
		} else {                                                                                                                     \
			constructed_definition_n.offsets[next_to_add_n] = sys::national_mod_offsets::X;                                            \
			constructed_definition_n.values[next_to_add_n] = v;                                                                        \
			++next_to_add_n;                                                                                                           \
		}                                                                                                                            \
	}

struct modifier_base {
protected:
	sys::national_modifier_definition constructed_definition_n;
	sys::provincial_modifier_definition constructed_definition_p;

public:
	uint32_t next_to_add_p = 0;
	uint32_t next_to_add_n = 0;
	uint8_t icon_index = 0;
	template<typename T>
	void icon(association_type, uint32_t v, error_handler& err, int32_t line, T& context) {
		icon_index = uint8_t(v);
	}

	MOD_PROV_FUNCTION(supply_limit)
	MOD_PROV_FUNCTION(attrition)
	MOD_PROV_FUNCTION(max_attrition)
	MOD_NAT_FUNCTION(war_exhaustion)
	MOD_NAT_FUNCTION(max_war_exhaustion)
	MOD_NAT_FUNCTION(leadership)
	MOD_NAT_FUNCTION(leadership_modifier)
	MOD_NAT_FUNCTION(supply_consumption)
	MOD_NAT_FUNCTION(org_regain)
	MOD_NAT_FUNCTION(reinforce_speed)
	MOD_NAT_FUNCTION(land_organisation)
	MOD_NAT_FUNCTION(naval_organisation)
	MOD_NAT_FUNCTION(research_points)
	MOD_NAT_FUNCTION(research_points_modifier)
	MOD_NAT_FUNCTION(research_points_on_conquer)
	MOD_NAT_FUNCTION(import_cost)
	MOD_NAT_FUNCTION(loan_interest)
	template<typename T>
	void tax_efficiency(association_type, float v, error_handler& err, int32_t line, T& context) {
		if(next_to_add_n >= sys::national_modifier_definition::modifier_definition_size) {
			err.accumulated_errors += "Too many modifier values; " + err.file_name + " line " + std::to_string(line) + "\n";
		} else {
			constructed_definition_n.offsets[next_to_add_n] = sys::national_mod_offsets::tax_efficiency;
			constructed_definition_n.values[next_to_add_n] = v * 0.01f;
			++next_to_add_n;
		}
	}
	MOD_NAT_FUNCTION(min_tax)
	MOD_NAT_FUNCTION(max_tax)
	MOD_NAT_FUNCTION(min_military_spending)
	MOD_NAT_FUNCTION(max_military_spending)
	MOD_NAT_FUNCTION(min_social_spending)
	MOD_NAT_FUNCTION(max_social_spending)
	MOD_NAT_FUNCTION(factory_owner_cost)
	MOD_NAT_FUNCTION(min_tariff)
	MOD_NAT_FUNCTION(max_tariff)
	MOD_NAT_FUNCTION(ruling_party_support)
	MOD_PROV_FUNCTION(local_ruling_party_support)
	MOD_NAT_FUNCTION(rich_vote)
	MOD_NAT_FUNCTION(middle_vote)
	MOD_NAT_FUNCTION(poor_vote)
	MOD_NAT_FUNCTION(minimum_wage)
	MOD_NAT_FUNCTION(factory_maintenance)
	MOD_PROV_FUNCTION(poor_life_needs)
	MOD_PROV_FUNCTION(rich_life_needs)
	MOD_PROV_FUNCTION(middle_life_needs)
	MOD_PROV_FUNCTION(poor_everyday_needs)
	MOD_PROV_FUNCTION(middle_everyday_needs)
	MOD_PROV_FUNCTION(rich_everyday_needs)
	MOD_PROV_FUNCTION(poor_luxury_needs)
	MOD_PROV_FUNCTION(middle_luxury_needs)
	MOD_PROV_FUNCTION(rich_luxury_needs)
	MOD_NAT_FUNCTION(unemployment_benefit)
	MOD_NAT_FUNCTION(pension_level)
	MOD_PROV_FUNCTION(population_growth)
	template<typename T>
	void global_population_growth(association_type, float v, error_handler& err, int32_t line, T& context) {
		if(next_to_add_n >= sys::national_modifier_definition::modifier_definition_size) {
			err.accumulated_errors += "Too many modifier values; " + err.file_name + " line " + std::to_string(line) + "\n";
		} else {
			constructed_definition_n.offsets[next_to_add_n] = sys::national_mod_offsets::pop_growth;
			constructed_definition_n.values[next_to_add_n] = v * 1.0f;
			++next_to_add_n;
		}
	}
	MOD_NAT_FUNCTION(factory_input)
	MOD_NAT_FUNCTION(factory_output)
	MOD_NAT_FUNCTION(factory_throughput)
	MOD_NAT_FUNCTION(rgo_input)
	MOD_NAT_FUNCTION(rgo_output)
	MOD_NAT_FUNCTION(rgo_throughput)
	MOD_NAT_FUNCTION(artisan_input)
	MOD_NAT_FUNCTION(artisan_output)
	MOD_NAT_FUNCTION(artisan_throughput)
	MOD_PROV_FUNCTION(local_factory_input)
	MOD_PROV_FUNCTION(local_factory_output)
	MOD_PROV_FUNCTION(local_factory_throughput)
	MOD_PROV_FUNCTION(local_rgo_input)
	MOD_PROV_FUNCTION(local_rgo_output)
	MOD_PROV_FUNCTION(local_rgo_throughput)
	MOD_PROV_FUNCTION(local_artisan_input)
	MOD_PROV_FUNCTION(local_artisan_output)
	MOD_PROV_FUNCTION(local_artisan_throughput)
	MOD_PROV_FUNCTION(number_of_voters)
	MOD_PROV_FUNCTION(goods_demand)
	MOD_NAT_FUNCTION(badboy)
	MOD_PROV_FUNCTION(assimilation_rate)
	MOD_NAT_FUNCTION(global_assimilation_rate)
	MOD_NAT_FUNCTION(prestige)
	MOD_NAT_FUNCTION(factory_cost)
	MOD_PROV_FUNCTION(life_rating)
	MOD_PROV_FUNCTION(farm_rgo_eff)
	MOD_PROV_FUNCTION(mine_rgo_eff)
	MOD_PROV_FUNCTION(farm_rgo_size)
	MOD_PROV_FUNCTION(mine_rgo_size)
	template<typename T>
	void m_rgo_size(association_type, float v, error_handler& err, int32_t line, T& context) {
		if(next_to_add_p >= sys::provincial_modifier_definition::modifier_definition_size) {
				err.accumulated_errors += "Too many modifier values; " + err.file_name + " line " + std::to_string(line) + "\n"; 
		} else {
			constructed_definition_p.offsets[next_to_add_p] = sys::provincial_mod_offsets::farm_rgo_size;
			constructed_definition_p.values[next_to_add_p] = v; 
			++next_to_add_p;
		}
		if(next_to_add_p >= sys::provincial_modifier_definition::modifier_definition_size) {
			err.accumulated_errors += "Too many modifier values; " + err.file_name + " line " + std::to_string(line) + "\n";
		} else {
			constructed_definition_p.offsets[next_to_add_p] = sys::provincial_mod_offsets::mine_rgo_size;
			constructed_definition_p.values[next_to_add_p] = v;
			++next_to_add_p;
		}
	}
	MOD_NAT_FUNCTION(issue_change_speed)
	MOD_NAT_FUNCTION(social_reform_desire)
	MOD_NAT_FUNCTION(political_reform_desire)
	MOD_NAT_FUNCTION(literacy_con_impact)
	MOD_PROV_FUNCTION(pop_militancy_modifier)
	MOD_PROV_FUNCTION(pop_consciousness_modifier)
	MOD_PROV_FUNCTION(rich_income_modifier)
	MOD_PROV_FUNCTION(middle_income_modifier)
	MOD_PROV_FUNCTION(poor_income_modifier)
	MOD_PROV_FUNCTION(boost_strongest_party)
	MOD_NAT_FUNCTION(global_immigrant_attract)
	template<typename T>
	void immigration(association_type, float v, error_handler& err, int32_t line, T& context) {
		if(next_to_add_p >= sys::provincial_modifier_definition::modifier_definition_size) {
			err.accumulated_errors += "Too many modifier values; " + err.file_name + " line " + std::to_string(line) + "\n";
		} else {
			constructed_definition_p.offsets[next_to_add_p] = sys::provincial_mod_offsets::immigrant_attract;
			constructed_definition_p.values[next_to_add_p] = v;
			++next_to_add_p;
		}
	}
	MOD_PROV_FUNCTION(immigrant_attract)
	MOD_PROV_FUNCTION(immigrant_push)
	MOD_PROV_FUNCTION(local_repair)
	MOD_PROV_FUNCTION(local_ship_build)
	MOD_NAT_FUNCTION(poor_savings_modifier)
	MOD_NAT_FUNCTION(influence_modifier)
	MOD_NAT_FUNCTION(diplomatic_points_modifier)
	MOD_NAT_FUNCTION(mobilization_size)
	MOD_NAT_FUNCTION(global_pop_militancy_modifier)
	MOD_NAT_FUNCTION(global_pop_consciousness_modifier)
	MOD_PROV_FUNCTION(movement_cost)
	MOD_PROV_FUNCTION(combat_width)
	MOD_PROV_FUNCTION(min_build_naval_base)
	MOD_PROV_FUNCTION(min_build_railroad)
	MOD_PROV_FUNCTION(min_build_fort)
	MOD_PROV_FUNCTION(min_build_bank)
	MOD_PROV_FUNCTION(min_build_university)
	MOD_PROV_FUNCTION(attack)
	template<typename T>
	void defender(association_type, float v, error_handler& err, int32_t line, T& context) {
		if(next_to_add_p >= sys::provincial_modifier_definition::modifier_definition_size) {
			err.accumulated_errors += "Too many modifier values; " + err.file_name + " line " + std::to_string(line) + "\n";
		} else {
			constructed_definition_p.offsets[next_to_add_p] = sys::provincial_mod_offsets::defense;
			constructed_definition_p.values[next_to_add_p] = v;
			++next_to_add_p;
		}
	}
	template<typename T>
	void attacker(association_type, float v, error_handler& err, int32_t line, T& context) {
		if(next_to_add_p >= sys::provincial_modifier_definition::modifier_definition_size) {
			err.accumulated_errors += "Too many modifier values; " + err.file_name + " line " + std::to_string(line) + "\n";
		} else {
			constructed_definition_p.offsets[next_to_add_p] = sys::provincial_mod_offsets::attack;
			constructed_definition_p.values[next_to_add_p] = v;
			++next_to_add_p;
		}
	}
	template<typename T>
	void defence(association_type, float v, error_handler& err, int32_t line, T& context) {
		if(next_to_add_p >= sys::provincial_modifier_definition::modifier_definition_size) {
			err.accumulated_errors += "Too many modifier values; " + err.file_name + " line " + std::to_string(line) + "\n";
		} else {
			constructed_definition_p.offsets[next_to_add_p] = sys::provincial_mod_offsets::defense;
			constructed_definition_p.values[next_to_add_p] = v;
			++next_to_add_p;
		}
	}
	MOD_NAT_FUNCTION(core_pop_militancy_modifier)
	MOD_NAT_FUNCTION(core_pop_consciousness_modifier)
	MOD_NAT_FUNCTION(non_accepted_pop_militancy_modifier)
	MOD_NAT_FUNCTION(non_accepted_pop_consciousness_modifier)
	MOD_NAT_FUNCTION(cb_generation_speed_modifier)
	MOD_NAT_FUNCTION(mobilization_impact)
	MOD_NAT_FUNCTION(suppression_points_modifier)
	MOD_NAT_FUNCTION(education_efficiency_modifier)
	MOD_NAT_FUNCTION(civilization_progress_modifier)
	MOD_NAT_FUNCTION(administrative_efficiency_modifier)
	MOD_NAT_FUNCTION(land_unit_start_experience)
	MOD_NAT_FUNCTION(naval_unit_start_experience)
	MOD_NAT_FUNCTION(naval_attack_modifier)
	MOD_NAT_FUNCTION(naval_defense_modifier)
	MOD_NAT_FUNCTION(land_attack_modifier)
	MOD_NAT_FUNCTION(land_defense_modifier)
	MOD_NAT_FUNCTION(tariff_efficiency_modifier)
	MOD_NAT_FUNCTION(max_loan_modifier)
	MOD_NAT_FUNCTION(unciv_economic_modifier)
	MOD_NAT_FUNCTION(unciv_military_modifier)
	MOD_NAT_FUNCTION(self_unciv_economic_modifier)
	MOD_NAT_FUNCTION(self_unciv_military_modifier)
	MOD_NAT_FUNCTION(commerce_tech_research_bonus)
	MOD_NAT_FUNCTION(army_tech_research_bonus)
	MOD_NAT_FUNCTION(industry_tech_research_bonus)
	MOD_NAT_FUNCTION(navy_tech_research_bonus)
	MOD_NAT_FUNCTION(culture_tech_research_bonus)
	MOD_NAT_FUNCTION(colonial_migration)
	MOD_NAT_FUNCTION(max_national_focus)
	MOD_NAT_FUNCTION(education_efficiency)
	MOD_NAT_FUNCTION(reinforce_rate)
	MOD_NAT_FUNCTION(influence)
	MOD_NAT_FUNCTION(dig_in_cap)
	MOD_NAT_FUNCTION(military_tactics)
	MOD_NAT_FUNCTION(supply_range)
	MOD_NAT_FUNCTION(regular_experience_level)
	MOD_NAT_FUNCTION(permanent_prestige)
	MOD_NAT_FUNCTION(soldier_to_pop_loss)
	MOD_NAT_FUNCTION(naval_attrition)
	MOD_NAT_FUNCTION(land_attrition)
	template<typename T>
	void pop_growth(association_type, float v, error_handler& err, int32_t line, T& context) {
		if(next_to_add_n >= sys::national_modifier_definition::modifier_definition_size) {
			err.accumulated_errors += "Too many modifier values; " + err.file_name + " line " + std::to_string(line) + "\n";
		} else {
			constructed_definition_n.offsets[next_to_add_n] = sys::national_mod_offsets::pop_growth;
			constructed_definition_n.values[next_to_add_n] = v * 10.0f;
			++next_to_add_n;
		}
	}
	MOD_NAT_FUNCTION(colonial_life_rating)
	MOD_NAT_FUNCTION(seperatism)
	MOD_NAT_FUNCTION(colonial_prestige)

	template<typename T>
	void unit_start_experience(association_type type, float v, error_handler& err, int32_t line, T& context) {
		land_unit_start_experience(type, v, err, line, context);
		naval_unit_start_experience(type, v, err, line, context);
	}

	template<typename T>
	void finish(T& context) { }

	sys::provincial_modifier_definition const& peek_province_mod() const {
		return constructed_definition_p;
	}
	sys::national_modifier_definition const& peek_national_mod() const {
		return constructed_definition_n;
	}
	sys::national_modifier_definition force_national_mod() const {
		sys::national_modifier_definition temp = constructed_definition_n;
		auto temp_next = next_to_add_n;
		for(uint32_t i = 0; i < sys::provincial_modifier_definition::modifier_definition_size; ++i) {
			if(constructed_definition_p.offsets[i] == sys::provincial_mod_offsets::poor_life_needs) {
				if(temp_next < sys::national_modifier_definition::modifier_definition_size) {
					temp.offsets[temp_next] = sys::national_mod_offsets::poor_life_needs;
					temp.values[temp_next] = constructed_definition_p.values[i];
					++temp_next;
				}
			} else if(constructed_definition_p.offsets[i] == sys::provincial_mod_offsets::middle_life_needs) {
				if(temp_next < sys::national_modifier_definition::modifier_definition_size) {
					temp.offsets[temp_next] = sys::national_mod_offsets::middle_life_needs;
					temp.values[temp_next] = constructed_definition_p.values[i];
					++temp_next;
				}
			} else if(constructed_definition_p.offsets[i] == sys::provincial_mod_offsets::rich_life_needs) {
				if(temp_next < sys::national_modifier_definition::modifier_definition_size) {
					temp.offsets[temp_next] = sys::national_mod_offsets::rich_life_needs;
					temp.values[temp_next] = constructed_definition_p.values[i];
					++temp_next;
				}
			} else if(constructed_definition_p.offsets[i] == sys::provincial_mod_offsets::poor_everyday_needs) {
				if(temp_next < sys::national_modifier_definition::modifier_definition_size) {
					temp.offsets[temp_next] = sys::national_mod_offsets::poor_everyday_needs;
					temp.values[temp_next] = constructed_definition_p.values[i];
					++temp_next;
				}
			} else if(constructed_definition_p.offsets[i] == sys::provincial_mod_offsets::middle_everyday_needs) {
				if(temp_next < sys::national_modifier_definition::modifier_definition_size) {
					temp.offsets[temp_next] = sys::national_mod_offsets::middle_everyday_needs;
					temp.values[temp_next] = constructed_definition_p.values[i];
					++temp_next;
				}
			} else if(constructed_definition_p.offsets[i] == sys::provincial_mod_offsets::rich_everyday_needs) {
				if(temp_next < sys::national_modifier_definition::modifier_definition_size) {
					temp.offsets[temp_next] = sys::national_mod_offsets::rich_everyday_needs;
					temp.values[temp_next] = constructed_definition_p.values[i];
					++temp_next;
				}
			} else if(constructed_definition_p.offsets[i] == sys::provincial_mod_offsets::poor_luxury_needs) {
				if(temp_next < sys::national_modifier_definition::modifier_definition_size) {
					temp.offsets[temp_next] = sys::national_mod_offsets::poor_luxury_needs;
					temp.values[temp_next] = constructed_definition_p.values[i];
					++temp_next;
				}
			} else if(constructed_definition_p.offsets[i] == sys::provincial_mod_offsets::middle_luxury_needs) {
				if(temp_next < sys::national_modifier_definition::modifier_definition_size) {
					temp.offsets[temp_next] = sys::national_mod_offsets::middle_luxury_needs;
					temp.values[temp_next] = constructed_definition_p.values[i];
					++temp_next;
				}
			} else if(constructed_definition_p.offsets[i] == sys::provincial_mod_offsets::rich_luxury_needs) {
				if(temp_next < sys::national_modifier_definition::modifier_definition_size) {
					temp.offsets[temp_next] = sys::national_mod_offsets::rich_luxury_needs;
					temp.values[temp_next] = constructed_definition_p.values[i];
					++temp_next;
				}
			} else if(constructed_definition_p.offsets[i] == sys::provincial_mod_offsets::goods_demand) {
				if(temp_next < sys::national_modifier_definition::modifier_definition_size) {
					temp.offsets[temp_next] = sys::national_mod_offsets::goods_demand;
					temp.values[temp_next] = constructed_definition_p.values[i];
					++temp_next;
				}
			} else if(constructed_definition_p.offsets[i] == sys::provincial_mod_offsets::farm_rgo_eff) {
				if(temp_next < sys::national_modifier_definition::modifier_definition_size) {
					temp.offsets[temp_next] = sys::national_mod_offsets::farm_rgo_eff;
					temp.values[temp_next] = constructed_definition_p.values[i];
					++temp_next;
				}
			} else if(constructed_definition_p.offsets[i] == sys::provincial_mod_offsets::mine_rgo_eff) {
				if(temp_next < sys::national_modifier_definition::modifier_definition_size) {
					temp.offsets[temp_next] = sys::national_mod_offsets::mine_rgo_eff;
					temp.values[temp_next] = constructed_definition_p.values[i];
					++temp_next;
				}
			} else if(constructed_definition_p.offsets[i] == sys::provincial_mod_offsets::farm_rgo_size) {
				if(temp_next < sys::national_modifier_definition::modifier_definition_size) {
					temp.offsets[temp_next] = sys::national_mod_offsets::farm_rgo_size;
					temp.values[temp_next] = constructed_definition_p.values[i];
					++temp_next;
				}
			} else if(constructed_definition_p.offsets[i] == sys::provincial_mod_offsets::mine_rgo_size) {
				if(temp_next < sys::national_modifier_definition::modifier_definition_size) {
					temp.offsets[temp_next] = sys::national_mod_offsets::mine_rgo_size;
					temp.values[temp_next] = constructed_definition_p.values[i];
					++temp_next;
				}
			} else if(constructed_definition_p.offsets[i] == sys::provincial_mod_offsets::rich_income_modifier) {
				if(temp_next < sys::national_modifier_definition::modifier_definition_size) {
					temp.offsets[temp_next] = sys::national_mod_offsets::rich_income_modifier;
					temp.values[temp_next] = constructed_definition_p.values[i];
					++temp_next;
				}
			} else if(constructed_definition_p.offsets[i] == sys::provincial_mod_offsets::middle_income_modifier) {
				if(temp_next < sys::national_modifier_definition::modifier_definition_size) {
					temp.offsets[temp_next] = sys::national_mod_offsets::middle_income_modifier;
					temp.values[temp_next] = constructed_definition_p.values[i];
					++temp_next;
				}
			} else if(constructed_definition_p.offsets[i] == sys::provincial_mod_offsets::poor_income_modifier) {
				if(temp_next < sys::national_modifier_definition::modifier_definition_size) {
					temp.offsets[temp_next] = sys::national_mod_offsets::poor_income_modifier;
					temp.values[temp_next] = constructed_definition_p.values[i];
					++temp_next;
				}
			} else if(constructed_definition_p.offsets[i] == sys::provincial_mod_offsets::combat_width) {
				if(temp_next < sys::national_modifier_definition::modifier_definition_size) {
					temp.offsets[temp_next] = sys::national_mod_offsets::combat_width;
					temp.values[temp_next] = constructed_definition_p.values[i];
					++temp_next;
				}
			} else if(constructed_definition_p.offsets[i] == sys::provincial_mod_offsets::assimilation_rate) {
				if(temp_next < sys::national_modifier_definition::modifier_definition_size) {
					temp.offsets[temp_next] = sys::national_mod_offsets::global_assimilation_rate;
					temp.values[temp_next] = constructed_definition_p.values[i];
					++temp_next;
				}
			} else if(constructed_definition_p.offsets[i] == sys::provincial_mod_offsets::pop_militancy_modifier) {
				if(temp_next < sys::national_modifier_definition::modifier_definition_size) {
					temp.offsets[temp_next] = sys::national_mod_offsets::global_pop_militancy_modifier;
					temp.values[temp_next] = constructed_definition_p.values[i];
					++temp_next;
				}
			} else if(constructed_definition_p.offsets[i] == sys::provincial_mod_offsets::pop_consciousness_modifier) {
				if(temp_next < sys::national_modifier_definition::modifier_definition_size) {
					temp.offsets[temp_next] = sys::national_mod_offsets::global_pop_consciousness_modifier;
					temp.values[temp_next] = constructed_definition_p.values[i];
					++temp_next;
				}
			}
		}
		return temp;
	}
};

#undef MOD_PROV_FUNCTION
#undef MOD_NAT_FUNCTION

struct int_vector {
	std::vector<int32_t> data;
	template<typename T>
	void free_value(int32_t v, error_handler& err, int32_t line, T& context) {
		data.push_back(v);
	}
	template<typename T>
	void finish(T& context) { }
};
struct commodity_array {
	tagged_vector<float, dcon::commodity_id> data;

	void any_value(std::string_view name, association_type, float value, error_handler& err, int32_t line,
			scenario_building_context& context) {
		auto found_commodity = context.map_of_commodity_names.find(std::string(name));
		if(found_commodity != context.map_of_commodity_names.end()) {
			data.safe_get(found_commodity->second) = value;
		} else {
			err.accumulated_errors +=
					"Unknown commodity " + std::string(name) + " in file " + err.file_name + " line " + std::to_string(line) + "\n";
		}
	}

	void finish(scenario_building_context& context);
};

struct building_definition : public modifier_base {
	int_vector colonial_points;
	commodity_array goods_cost;
	bool default_enabled = false;
	std::string_view production_type;
	float infrastructure = 0.0f;
	int32_t colonial_range = 0;
	int32_t max_level = 0;
	int32_t naval_capacity = 0;
	int32_t time = 0;
	int32_t cost = 0;
	economy::province_building_type stored_type = economy::province_building_type::factory;

	void type(association_type, std::string_view value, error_handler& err, int32_t line, scenario_building_context& context);
	void finish(scenario_building_context& context) { }
};

struct building_file {
	void result(std::string_view name, building_definition&& res, error_handler& err, int32_t line,
			scenario_building_context& context);
	void finish(scenario_building_context& context) { }
};

struct ideology_group_context {
	scenario_building_context& outer_context;
	dcon::ideology_group_id id;
};
struct ideology_group {
	void finish(ideology_group_context& context) { }
};

struct ideology_file {
	void finish(scenario_building_context& context) { }
};

void register_ideology(std::string_view name, token_generator& gen, error_handler& err, ideology_group_context& context);
void make_ideology_group(std::string_view name, token_generator& gen, error_handler& err, culture_group_context& context);

struct issue_context {
	scenario_building_context& outer_context;
	dcon::issue_id id;
};
struct reform_context {
	scenario_building_context& outer_context;
	dcon::reform_id id;
};
struct issue_group_context {
	scenario_building_context& outer_context;
	::culture::issue_category issue_cat = ::culture::issue_category::party;

	issue_group_context(scenario_building_context& outer_context, ::culture::issue_category issue_cat)
			: outer_context(outer_context), issue_cat(issue_cat) { }
};
struct issue {
	void next_step_only(association_type, bool value, error_handler& err, int32_t line, issue_context& context);
	void administrative(association_type, bool value, error_handler& err, int32_t line, issue_context& context);
	void finish(issue_context&) { }
	void next_step_only(association_type, bool value, error_handler& err, int32_t line, reform_context& context);
	void administrative(association_type, bool value, error_handler& err, int32_t line, reform_context& context);
	void finish(reform_context&) { }
};
struct issues_group {
	void finish(issue_group_context&) { }
};
struct issues_file {
	void finish(scenario_building_context&) { }
};

void make_issues_group(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);
void make_issue(std::string_view name, token_generator& gen, error_handler& err, issue_group_context& context);
void register_option(std::string_view name, token_generator& gen, error_handler& err, issue_context& context);
void register_option(std::string_view name, token_generator& gen, error_handler& err, reform_context& context);

struct government_type_context {
	scenario_building_context& outer_context;
	dcon::government_type_id id;
};
struct government_type {
	void election(association_type, bool value, error_handler& err, int32_t line, government_type_context& context);
	void appoint_ruling_party(association_type, bool value, error_handler& err, int32_t line, government_type_context& context);
	void duration(association_type, int32_t value, error_handler& err, int32_t line, government_type_context& context);
	void flagtype(association_type, std::string_view value, error_handler& err, int32_t line, government_type_context& context);
	void any_value(std::string_view text, association_type, bool value, error_handler& err, int32_t line,
			government_type_context& context);

	void finish(government_type_context&) { }
};

struct governments_file {
	void finish(scenario_building_context&) { }
};

void make_government(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);

struct cb_list {
	void free_value(std::string_view text, error_handler& err, int32_t line, scenario_building_context& context);
	void finish(scenario_building_context&) { }
};

void register_cb_type(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);

struct cb_types_file {
	cb_list peace_order;
	void finish(scenario_building_context&) { }
};

struct trait_context {
	scenario_building_context& outer_context;
	dcon::leader_trait_id id;
};

struct trait {
	void organisation(association_type, float value, error_handler& err, int32_t line, trait_context& context);
	void morale(association_type, float value, error_handler& err, int32_t line, trait_context& context);
	void attack(association_type, float value, error_handler& err, int32_t line, trait_context& context);
	void defence(association_type, float value, error_handler& err, int32_t line, trait_context& context);
	void reconnaissance(association_type, float value, error_handler& err, int32_t line, trait_context& context);
	void speed(association_type, float value, error_handler& err, int32_t line, trait_context& context);
	void experience(association_type, float value, error_handler& err, int32_t line, trait_context& context);
	void reliability(association_type, float value, error_handler& err, int32_t line, trait_context& context);
	void finish(trait_context&) { }
};

struct traits_set {
	void finish(scenario_building_context&) { }
};

struct traits_file {
	void finish(scenario_building_context&) { }
};

void make_trait(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);
void personality_traits_set(token_generator& gen, error_handler& err, scenario_building_context& context);
void background_traits_set(token_generator& gen, error_handler& err, scenario_building_context& context);

void register_crime(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);

struct crimes_file {
	void finish(scenario_building_context&) { }
};

struct triggered_modifier_context {
	scenario_building_context& outer_context;
	uint32_t index = 0;
	std::string_view name;
	triggered_modifier_context(scenario_building_context& outer_context, uint32_t index, std::string_view name)
			: outer_context(outer_context), index(index), name(name) { }
};

struct triggered_modifier : public modifier_base {
	void finish(triggered_modifier_context&);
};

struct triggered_modifiers_file {
	void finish(scenario_building_context&) { }
};

void register_trigger(token_generator& gen, error_handler& err, triggered_modifier_context& context);
void make_triggered_modifier(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);

void make_national_value(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);

struct national_values_file {
	void finish(scenario_building_context&) { }
};

void m_very_easy_player(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_easy_player(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_hard_player(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_very_easy_ai(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_easy_ai(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_hard_ai(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_very_hard_ai(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_overseas(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_coastal(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_non_coastal(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_coastal_sea(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_sea_zone(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_land_province(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_blockaded(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_no_adjacent_controlled(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_core(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_has_siege(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_occupied(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_nationalism(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_infrastructure(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_base_values(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_war(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_peace(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_disarming(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_war_exhaustion(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_badboy(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_debt_default_to(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_bad_debter(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_great_power(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_second_power(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_civ_nation(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_unciv_nation(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_average_literacy(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_plurality(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_generalised_debt_default(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_total_occupation(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_total_blockaded(token_generator& gen, error_handler& err, scenario_building_context& context);
void m_in_bankrupcy(token_generator& gen, error_handler& err, scenario_building_context& context);

struct static_modifiers_file {
	void finish(scenario_building_context&) { }
};

void make_event_modifier(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);

struct event_modifiers_file {
	void finish(scenario_building_context&) { }
};

void register_rebel_type(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);

struct rebel_types_file {
	void finish(scenario_building_context&) { }
};

struct sea_list {
	void free_value(int32_t value, error_handler& err, int32_t line, scenario_building_context& context);
	void finish(scenario_building_context&) { }
};

struct default_map_file {
	sea_list sea_starts;

	void max_provinces(association_type, int32_t value, error_handler& err, int32_t line, scenario_building_context& context);
	void finish(scenario_building_context&);
};

void read_map_colors(char const* start, char const* end, error_handler& err, scenario_building_context& context);
void read_map_adjacency(char const* start, char const* end, error_handler& err, scenario_building_context& context);

struct terrain_modifier : public modifier_base {
	color_from_3i color;
	void finish(scenario_building_context&) { }
};

struct terrain_modifiers_list {
	void finish(scenario_building_context&) { }
};

struct select_an_int {
	int32_t free_value = 0;
	void finish(scenario_building_context&) { }
};

struct palette_definition {
	std::string_view type;
	select_an_int color;
	void finish(scenario_building_context& context);
};

struct terrain_file {
	terrain_modifiers_list categories;
	palette_definition any_group;
	void finish(scenario_building_context&) { }
};

void make_terrain_modifier(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);

struct state_def_building_context {
	scenario_building_context& outer_context;
	dcon::state_definition_id id;
};

struct state_definition {
	void free_value(int32_t value, error_handler& err, int32_t line, state_def_building_context& context);
	void finish(state_def_building_context&) { }
};

struct region_file {
	void finish(scenario_building_context&) { }
};

void make_state_definition(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);

struct region_building_context {
	scenario_building_context& outer_context;
	dcon::region_id id;
};

struct region_definition {
	void free_value(int32_t value, error_handler& err, int32_t line, region_building_context& context);
	void finish(region_building_context&) { }
};

struct superregion_file {
	void finish(scenario_building_context&) { }
};

void make_region_definition(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);

struct continent_building_context {
	scenario_building_context& outer_context;
	dcon::modifier_id id;
};

struct continent_provinces {
	void free_value(int32_t value, error_handler& err, int32_t line, continent_building_context& context);
	void finish(continent_building_context&) { }
};

struct continent_definition : public modifier_base {
	continent_provinces provinces;
	void finish(continent_building_context&) { }
};

struct continent_file {
	void finish(scenario_building_context&) { }
};

void make_continent_definition(std::string_view name, token_generator& gen, error_handler& err,
		scenario_building_context& context);

struct climate_building_context {
	scenario_building_context& outer_context;
	dcon::modifier_id id;
};

struct climate_definition : public modifier_base {
	void free_value(int32_t value, error_handler& err, int32_t line, climate_building_context& context);
	void finish(climate_building_context&) { }
};

struct climate_file {
	void finish(scenario_building_context&) { }
};

void make_climate_definition(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);

struct tech_group_context {
	scenario_building_context& outer_context;
	::culture::tech_category category = ::culture::tech_category::unknown;
	tech_group_context(scenario_building_context& outer_context, ::culture::tech_category category)
			: outer_context(outer_context), category(category) { }
};
struct tech_folder_list {
	void free_value(std::string_view name, error_handler& err, int32_t line, tech_group_context& context);
	void finish(tech_group_context&) { }
};
struct tech_groups_list {
	void finish(scenario_building_context&) { }
};
struct tech_schools_list {
	void finish(scenario_building_context&) { }
};
struct technology_main_file {
	tech_groups_list folders;
	tech_schools_list schools;
	void finish(scenario_building_context&) { }
};
struct technology_sub_file {
	void finish(scenario_building_context&) { }
};

void make_tech_folder_list(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);
void read_school_modifier(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);
void register_technology(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);

struct inventions_file {
	void finish(tech_group_context&) { }
};

void register_invention(std::string_view name, token_generator& gen, error_handler& err, tech_group_context& context); // but not at the patent office

struct commodity_set : public economy::commodity_set {
	int32_t num_added = 0;
	void any_value(std::string_view name, association_type, float value, error_handler& err, int32_t line,
			scenario_building_context& context);

	void finish(scenario_building_context&) { }
};

struct unit_definition : public military::unit_definition {
	void unit_type_text(association_type, std::string_view value, error_handler& err, int32_t line,
			scenario_building_context& context) {
		if(is_fixed_token_ci(value.data(), value.data() + value.length(), "support"))
			type = military::unit_type::support;
		else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "big_ship"))
			type = military::unit_type::big_ship;
		else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "cavalry"))
			type = military::unit_type::cavalry;
		else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "transport"))
			type = military::unit_type::transport;
		else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "light_ship"))
			type = military::unit_type::light_ship;
		else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "special"))
			type = military::unit_type::special;
		else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "infantry"))
			type = military::unit_type::infantry;
	}
	void type_text(association_type, std::string_view value, error_handler& err, int32_t line, scenario_building_context& context) {
		if(is_fixed_token_ci(value.data(), value.data() + value.length(), "land"))
			is_land = true;
		else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "naval"))
			is_land = false;
		else
			err.accumulated_errors +=
					std::string(value) + " is not a valid unit type (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
	void finish(scenario_building_context&) { }
};

struct unit_file {
	void finish(scenario_building_context&) { }
};

void make_base_units(scenario_building_context& context);
void make_unit(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);

struct country_file_context {
	scenario_building_context& outer_context;
	dcon::national_identity_id id;
};

struct party_context {
	scenario_building_context& outer_context;
	dcon::political_party_id id;
};
struct unit_names_context {
	scenario_building_context& outer_context;
	dcon::national_identity_id id;
	dcon::unit_type_id u_id;
};

struct party {
	void ideology(association_type, std::string_view text, error_handler& err, int32_t line, party_context& context);
	void name(association_type, std::string_view text, error_handler& err, int32_t line, party_context& context);
	void start_date(association_type, sys::year_month_day ymd, error_handler& err, int32_t line, party_context& context);
	void end_date(association_type, sys::year_month_day ymd, error_handler& err, int32_t line, party_context& context);
	void any_value(std::string_view issue, association_type, std::string_view option, error_handler& err, int32_t line, party_context& context);
	void finish(party_context&) { }
};
struct unit_names_list {
	void free_value(std::string_view text, error_handler& err, int32_t line, unit_names_context& context);
	void finish(unit_names_context&) { }
};
struct unit_names_collection {
	void finish(country_file_context&) { }
};
struct country_file {
	void color(color_from_3i cvalue, error_handler& err, int32_t line, country_file_context& context);
	unit_names_collection unit_names;
	void any_group(std::string_view name, color_from_3i, error_handler& err, int32_t line, country_file_context& context);
	void finish(country_file_context&) { }
};

void make_party(token_generator& gen, error_handler& err, country_file_context& context);
void make_unit_names_list(std::string_view name, token_generator& gen, error_handler& err, country_file_context& context);

struct province_file_context {
	scenario_building_context& outer_context;
	dcon::province_id id;
};

struct pv_party_loyalty {
	int32_t loyalty_value = 0;
	dcon::ideology_id id;
	void ideology(association_type, std::string_view text, error_handler& err, int32_t line, province_file_context& context) {
		if(auto it = context.outer_context.map_of_ideologies.find(std::string(text));
				it != context.outer_context.map_of_ideologies.end()) {
			id = it->second.id;
		} else {
			err.accumulated_errors +=
					std::string(text) + " is not a valid ideology name (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void finish(province_file_context&) { }
};
struct pv_state_building {
	int32_t level = 1;
	dcon::factory_type_id id;
	void building(association_type, std::string_view text, error_handler& err, int32_t line, province_file_context& context) {
		if(auto it = context.outer_context.map_of_factory_names.find(std::string(text));
				it != context.outer_context.map_of_factory_names.end()) {
			id = it->second;
		} else {
			err.accumulated_errors +=
					std::string(text) + " is not a valid factory name (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void finish(province_file_context&) { }
};

struct province_history_file {
	void life_rating(association_type, uint32_t value, error_handler& err, int32_t line, province_file_context& context);
	void colony(association_type, uint32_t value, error_handler& err, int32_t line, province_file_context& context);
	void trade_goods(association_type, std::string_view text, error_handler& err, int32_t line, province_file_context& context);
	void owner(association_type, uint32_t value, error_handler& err, int32_t line, province_file_context& context);
	void controller(association_type, uint32_t value, error_handler& err, int32_t line, province_file_context& context);
	void terrain(association_type, std::string_view text, error_handler& err, int32_t line, province_file_context& context);
	void add_core(association_type, uint32_t value, error_handler& err, int32_t line, province_file_context& context);
	void remove_core(association_type, uint32_t value, error_handler& err, int32_t line, province_file_context& context);
	void party_loyalty(pv_party_loyalty const& value, error_handler& err, int32_t line, province_file_context& context);
	void state_building(pv_state_building const& value, error_handler& err, int32_t line, province_file_context& context);
	void is_slave(association_type, bool value, error_handler& err, int32_t line, province_file_context& context);
	void any_value(std::string_view name, association_type, uint32_t value, error_handler& err, int32_t line,
			province_file_context& context);
	void finish(province_file_context&) { }
};

void enter_dated_block(std::string_view name, token_generator& gen, error_handler& err, province_file_context& context);

struct pop_history_province_context {
	scenario_building_context& outer_context;
	dcon::province_id id;
};

struct pop_history_definition {
	dcon::religion_id rel_id;
	dcon::culture_id cul_id;
	dcon::rebel_type_id reb_id;
	int32_t size = 0;
	float militancy = 0;
	void culture(association_type, std::string_view value, error_handler& err, int32_t line, pop_history_province_context& context);
	void religion(association_type, std::string_view value, error_handler& err, int32_t line,
			pop_history_province_context& context);
	void rebel_type(association_type, std::string_view value, error_handler& err, int32_t line,
			pop_history_province_context& context);
	void finish(pop_history_province_context&) { }
};

struct pop_province_list {
	void any_group(std::string_view type, pop_history_definition const& def, error_handler& err, int32_t line,
			pop_history_province_context& context);
	void finish(pop_history_province_context&) { }
};

struct pop_history_file {
	void finish(scenario_building_context&) { }
};

void make_pop_province_list(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);

struct poptype_context {
	scenario_building_context& outer_context;
	dcon::pop_type_id id;
};

struct promotion_targets {
	void finish(poptype_context&) { }
};

struct pop_ideologies {
	void finish(poptype_context&) { }
};

struct pop_issues {
	void finish(poptype_context&) { }
};

struct income {
	float weight = 0;
	::culture::income_type itype = ::culture::income_type::none;
	void type(association_type, std::string_view value, error_handler& err, int32_t line, poptype_context& context) {
		if(is_fixed_token_ci(value.data(), value.data() + value.length(), "administration"))
			itype = ::culture::income_type::administration;
		else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "military"))
			itype = ::culture::income_type::military;
		else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "education"))
			itype = ::culture::income_type::education;
		else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "reforms"))
			itype = ::culture::income_type::reforms;
		else {
			err.accumulated_errors +=
					"Invalid income type " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void finish(poptype_context&) { }
};

struct poptype_file {
	void finish(poptype_context&) { }
	void sprite(association_type, int32_t value, error_handler& err, int32_t line, poptype_context& context);
	void color(color_from_3i cvalue, error_handler& err, int32_t line, poptype_context& context);
	void is_artisan(association_type, bool value, error_handler& err, int32_t line, poptype_context& context);
	void strata(association_type, std::string_view value, error_handler& err, int32_t line, poptype_context& context);
	void unemployment(association_type, bool value, error_handler& err, int32_t line, poptype_context& context);
	void is_slave(association_type, bool value, error_handler& err, int32_t line, poptype_context& context);
	void allowed_to_vote(association_type, bool value, error_handler& err, int32_t line, poptype_context& context);
	void can_be_recruited(association_type, bool value, error_handler& err, int32_t line, poptype_context& context);
	void state_capital_only(association_type, bool value, error_handler& err, int32_t line, poptype_context& context);
	void leadership(association_type, int32_t value, error_handler& err, int32_t line, poptype_context& context);
	void research_optimum(association_type, float value, error_handler& err, int32_t line, poptype_context& context);
	void administrative_efficiency(association_type, bool value, error_handler& err, int32_t line, poptype_context& context);
	void tax_eff(association_type, float value, error_handler& err, int32_t line, poptype_context& context);
	void can_build(association_type, bool value, error_handler& err, int32_t line, poptype_context& context);
	void research_points(association_type, float value, error_handler& err, int32_t line, poptype_context& context);
	void can_reduce_consciousness(association_type, bool value, error_handler& err, int32_t line, poptype_context& context);
	void workplace_input(association_type, float value, error_handler& err, int32_t line, poptype_context& context);
	void workplace_output(association_type, float value, error_handler& err, int32_t line, poptype_context& context);
	void equivalent(association_type, std::string_view value, error_handler& err, int32_t line, poptype_context& context);
	void life_needs(commodity_array const& value, error_handler& err, int32_t line, poptype_context& context);
	void everyday_needs(commodity_array const& value, error_handler& err, int32_t line, poptype_context& context);
	void luxury_needs(commodity_array const& value, error_handler& err, int32_t line, poptype_context& context);
	promotion_targets promote_to;
	pop_ideologies ideologies;
	pop_issues issues;

	void life_needs_income(income const& value, error_handler& err, int32_t line, poptype_context& context);
	void everyday_needs_income(income const& value, error_handler& err, int32_t line, poptype_context& context);
	void luxury_needs_income(income const& value, error_handler& err, int32_t line, poptype_context& context);
};

commodity_array stub_commodity_array(token_generator& gen, error_handler& err, poptype_context& context);
void read_promotion_target(std::string_view name, token_generator& gen, error_handler& err, poptype_context& context);
void read_pop_ideology(std::string_view name, token_generator& gen, error_handler& err, poptype_context& context);
void read_pop_issue(std::string_view name, token_generator& gen, error_handler& err, poptype_context& context);
void read_c_migration_target(token_generator& gen, error_handler& err, poptype_context& context);
void read_migration_target(token_generator& gen, error_handler& err, poptype_context& context);

struct individual_ideology_context {
	scenario_building_context& outer_context;
	dcon::ideology_id id;
};

struct individual_ideology {
	void finish(individual_ideology_context&) { }
	void can_reduce_militancy(association_type, bool value, error_handler& err, int32_t line, individual_ideology_context& context);
	void uncivilized(association_type, bool value, error_handler& err, int32_t line, individual_ideology_context& context);
	void civilized(association_type, bool value, error_handler& err, int32_t line, individual_ideology_context& context);
	void color(color_from_3i cvalue, error_handler& err, int32_t line, individual_ideology_context& context);
	void date(association_type, sys::year_month_day ymd, error_handler& err, int32_t line, individual_ideology_context& context);
	void add_political_reform(dcon::value_modifier_key value, error_handler& err, int32_t line,
			individual_ideology_context& context);
	void remove_political_reform(dcon::value_modifier_key value, error_handler& err, int32_t line,
			individual_ideology_context& context);
	void add_social_reform(dcon::value_modifier_key value, error_handler& err, int32_t line, individual_ideology_context& context);
	void remove_social_reform(dcon::value_modifier_key value, error_handler& err, int32_t line,
			individual_ideology_context& context);
	void add_military_reform(dcon::value_modifier_key value, error_handler& err, int32_t line,
			individual_ideology_context& context);
	void add_economic_reform(dcon::value_modifier_key value, error_handler& err, int32_t line,
			individual_ideology_context& context);
};

dcon::value_modifier_key ideology_condition(token_generator& gen, error_handler& err, individual_ideology_context& context);
dcon::trigger_key read_triggered_modifier_condition(token_generator& gen, error_handler& err, scenario_building_context& context);

struct individual_cb_context {
	scenario_building_context& outer_context;
	dcon::cb_type_id id;
};

struct cb_body {
	void finish(individual_cb_context& context);
	void is_civil_war(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context);
	void months(association_type, int32_t value, error_handler& err, int32_t line, individual_cb_context& context);
	void truce_months(association_type, int32_t value, error_handler& err, int32_t line, individual_cb_context& context);
	void sprite_index(association_type, int32_t value, error_handler& err, int32_t line, individual_cb_context& context);
	void always(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context);
	void is_triggered_only(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context);
	void constructing_cb(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context);
	void great_war_obligatory(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context);
	void all_allowed_states(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context);
	void crisis(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context);
	void po_clear_union_sphere(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context);
	void po_gunboat(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context);
	void po_annex(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context);
	void po_demand_state(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context);
	void po_add_to_sphere(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context);
	void po_disarmament(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context);
	void po_reparations(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context);
	void po_transfer_provinces(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context);
	void po_remove_prestige(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context);
	void po_make_puppet(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context);
	void po_release_puppet(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context);
	void po_status_quo(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context);
	void po_install_communist_gov_type(association_type, bool value, error_handler& err, int32_t line,
			individual_cb_context& context);
	void po_uninstall_communist_gov_type(association_type, bool value, error_handler& err, int32_t line,
			individual_cb_context& context);
	void po_remove_cores(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context);
	void po_colony(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context);
	void po_destroy_forts(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context);
	void po_destroy_naval_bases(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context);
	void war_name(association_type, std::string_view value, error_handler& err, int32_t line, individual_cb_context& context);
	void badboy_factor(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context);
	void prestige_factor(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context);
	void peace_cost_factor(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context);
	void penalty_factor(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context);
	void break_truce_prestige_factor(association_type, float value, error_handler& err, int32_t line,
			individual_cb_context& context);
	void break_truce_infamy_factor(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context);
	void break_truce_militancy_factor(association_type, float value, error_handler& err, int32_t line,
			individual_cb_context& context);
	void good_relation_prestige_factor(association_type, float value, error_handler& err, int32_t line,
			individual_cb_context& context);
	void good_relation_infamy_factor(association_type, float value, error_handler& err, int32_t line,
			individual_cb_context& context);
	void good_relation_militancy_factor(association_type, float value, error_handler& err, int32_t line,
			individual_cb_context& context);
	void construction_speed(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context);
	void tws_battle_factor(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context);
	void allowed_states(dcon::trigger_key value, error_handler& err, int32_t line, individual_cb_context& context);
	void allowed_states_in_crisis(dcon::trigger_key value, error_handler& err, int32_t line, individual_cb_context& context);
	void allowed_substate_regions(dcon::trigger_key value, error_handler& err, int32_t line, individual_cb_context& context);
	void allowed_countries(dcon::trigger_key value, error_handler& err, int32_t line, individual_cb_context& context);
	void can_use(dcon::trigger_key value, error_handler& err, int32_t line, individual_cb_context& context);
	void on_add(dcon::effect_key value, error_handler& err, int32_t line, individual_cb_context& context);
	void on_po_accepted(dcon::effect_key value, error_handler& err, int32_t line, individual_cb_context& context);
};

dcon::trigger_key cb_allowed_states(token_generator& gen, error_handler& err, individual_cb_context& context);
dcon::trigger_key cb_allowed_crisis_states(token_generator& gen, error_handler& err, individual_cb_context& context);
dcon::trigger_key cb_allowed_substates(token_generator& gen, error_handler& err, individual_cb_context& context);
dcon::trigger_key cb_allowed_countries(token_generator& gen, error_handler& err, individual_cb_context& context);
dcon::trigger_key cb_can_use(token_generator& gen, error_handler& err, individual_cb_context& context);
dcon::effect_key cb_on_add(token_generator& gen, error_handler& err, individual_cb_context& context);
dcon::effect_key cb_on_po_accepted(token_generator& gen, error_handler& err, individual_cb_context& context);

struct crime_modifier : public modifier_base {
	bool active = false;
	dcon::trigger_key trigger;
};

dcon::trigger_key make_crime_trigger(token_generator& gen, error_handler& err, scenario_building_context& context);
void read_pending_crime(dcon::crime_id id, token_generator& gen, error_handler& err, scenario_building_context& context);

struct individual_option_context {
	scenario_building_context& outer_context;
	dcon::issue_option_id id;
};

struct individual_roption_context {
	scenario_building_context& outer_context;
	dcon::reform_option_id id;
};

struct option_rules {
	void finish(individual_option_context&) { }
	void build_factory(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context);
	void expand_factory(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context);
	void open_factory(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context);
	void destroy_factory(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context);
	void factory_priority(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context);
	void can_subsidise(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context);
	void pop_build_factory(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context);
	void pop_expand_factory(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context);
	void pop_open_factory(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context);
	void delete_factory_if_no_input(association_type, bool value, error_handler& err, int32_t line,
			individual_option_context& context);
	void build_factory_invest(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context);
	void expand_factory_invest(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context);
	void open_factory_invest(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context);
	void build_railway_invest(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context);
	void can_invest_in_pop_projects(association_type, bool value, error_handler& err, int32_t line,
			individual_option_context& context);
	void pop_build_factory_invest(association_type, bool value, error_handler& err, int32_t line,
			individual_option_context& context);
	void pop_expand_factory_invest(association_type, bool value, error_handler& err, int32_t line,
			individual_option_context& context);
	void pop_open_factory_invest(association_type, bool value, error_handler& err, int32_t line,
			individual_option_context& context);
	void allow_foreign_investment(association_type, bool value, error_handler& err, int32_t line,
			individual_option_context& context);
	void slavery_allowed(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context);
	void primary_culture_voting(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context);
	void culture_voting(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context);
	void all_voting(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context);
	void largest_share(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context);
	void dhont(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context);
	void sainte_laque(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context);
	void same_as_ruling_party(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context);
	void rich_only(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context);
	void state_vote(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context);
	void population_vote(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context);
	void build_railway(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context);
	void build_bank(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context);
	void build_university(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context);

	void finish(individual_roption_context&) { }
	void build_factory(association_type, bool value, error_handler& err, int32_t line, individual_roption_context& context);
	void expand_factory(association_type, bool value, error_handler& err, int32_t line, individual_roption_context& context);
	void open_factory(association_type, bool value, error_handler& err, int32_t line, individual_roption_context& context);
	void destroy_factory(association_type, bool value, error_handler& err, int32_t line, individual_roption_context& context);
	void factory_priority(association_type, bool value, error_handler& err, int32_t line, individual_roption_context& context);
	void can_subsidise(association_type, bool value, error_handler& err, int32_t line, individual_roption_context& context);
	void pop_build_factory(association_type, bool value, error_handler& err, int32_t line, individual_roption_context& context);
	void pop_expand_factory(association_type, bool value, error_handler& err, int32_t line, individual_roption_context& context);
	void pop_open_factory(association_type, bool value, error_handler& err, int32_t line, individual_roption_context& context);
	void delete_factory_if_no_input(association_type, bool value, error_handler& err, int32_t line,
			individual_roption_context& context);
	void build_factory_invest(association_type, bool value, error_handler& err, int32_t line, individual_roption_context& context);
	void expand_factory_invest(association_type, bool value, error_handler& err, int32_t line, individual_roption_context& context);
	void open_factory_invest(association_type, bool value, error_handler& err, int32_t line, individual_roption_context& context);
	void build_railway_invest(association_type, bool value, error_handler& err, int32_t line, individual_roption_context& context);
	void can_invest_in_pop_projects(association_type, bool value, error_handler& err, int32_t line,
			individual_roption_context& context);
	void pop_build_factory_invest(association_type, bool value, error_handler& err, int32_t line,
			individual_roption_context& context);
	void pop_expand_factory_invest(association_type, bool value, error_handler& err, int32_t line,
			individual_roption_context& context);
	void pop_open_factory_invest(association_type, bool value, error_handler& err, int32_t line,
			individual_roption_context& context);
	void allow_foreign_investment(association_type, bool value, error_handler& err, int32_t line,
			individual_roption_context& context);
	void slavery_allowed(association_type, bool value, error_handler& err, int32_t line, individual_roption_context& context);
	void primary_culture_voting(association_type, bool value, error_handler& err, int32_t line,
			individual_roption_context& context);
	void culture_voting(association_type, bool value, error_handler& err, int32_t line, individual_roption_context& context);
	void all_voting(association_type, bool value, error_handler& err, int32_t line, individual_roption_context& context);
	void largest_share(association_type, bool value, error_handler& err, int32_t line, individual_roption_context& context);
	void dhont(association_type, bool value, error_handler& err, int32_t line, individual_roption_context& context);
	void sainte_laque(association_type, bool value, error_handler& err, int32_t line, individual_roption_context& context);
	void same_as_ruling_party(association_type, bool value, error_handler& err, int32_t line, individual_roption_context& context);
	void rich_only(association_type, bool value, error_handler& err, int32_t line, individual_roption_context& context);
	void state_vote(association_type, bool value, error_handler& err, int32_t line, individual_roption_context& context);
	void population_vote(association_type, bool value, error_handler& err, int32_t line, individual_roption_context& context);
	void build_railway(association_type, bool value, error_handler& err, int32_t line, individual_roption_context& context);
	void build_bank(association_type, bool value, error_handler& err, int32_t line, individual_roption_context& context);
	void build_university(association_type, bool value, error_handler& err, int32_t line, individual_roption_context& context);
};

struct on_execute_body {
	dcon::trigger_key trigger;
	dcon::effect_key effect;
	void finish(individual_option_context&) { }
	void finish(individual_roption_context&) { }
};

struct issue_option_body : public modifier_base {
	void technology_cost(association_type, int32_t value, error_handler& err, int32_t line, individual_option_context& context);
	void war_exhaustion_effect(association_type, float value, error_handler& err, int32_t line, individual_option_context& context);
	void administrative_multiplier(association_type, float value, error_handler& err, int32_t line,
			individual_option_context& context);
	void is_jingoism(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context);
	void on_execute(on_execute_body const& value, error_handler& err, int32_t line, individual_option_context& context);

	void technology_cost(association_type, int32_t value, error_handler& err, int32_t line, individual_roption_context& context);
	void war_exhaustion_effect(association_type, float value, error_handler& err, int32_t line,
			individual_roption_context& context);
	void administrative_multiplier(association_type, float value, error_handler& err, int32_t line,
			individual_roption_context& context);
	void on_execute(on_execute_body const& value, error_handler& err, int32_t line, individual_roption_context& context);
	void is_jingoism(association_type, bool value, error_handler& err, int32_t line, individual_roption_context& context) { }
	option_rules rules;
};

void make_opt_allow(token_generator& gen, error_handler& err, individual_option_context& context);
dcon::trigger_key make_execute_trigger(token_generator& gen, error_handler& err, individual_option_context& context);
dcon::effect_key make_execute_effect(token_generator& gen, error_handler& err, individual_option_context& context);
void make_opt_allow(token_generator& gen, error_handler& err, individual_roption_context& context);
dcon::trigger_key make_execute_trigger(token_generator& gen, error_handler& err, individual_roption_context& context);
dcon::effect_key make_execute_effect(token_generator& gen, error_handler& err, individual_roption_context& context);
void read_pending_option(dcon::issue_option_id id, token_generator& gen, error_handler& err, scenario_building_context& context);
void read_pending_reform(dcon::reform_option_id id, token_generator& gen, error_handler& err, scenario_building_context& context);

struct national_focus_context {
	scenario_building_context& outer_context;
	dcon::national_focus_id id;
	::nations::focus_type type = ::nations::focus_type::unknown;
};

struct national_focus : public modifier_base {
	void finish(national_focus_context&) { }
	void railroads(association_type, float value, error_handler& err, int32_t line, national_focus_context& context);
	void limit(dcon::trigger_key value, error_handler& err, int32_t line, national_focus_context& context);
	void has_flashpoint(association_type, bool value, error_handler& err, int32_t line, national_focus_context& context);
	void flashpoint_tension(association_type, float value, error_handler& err, int32_t line, national_focus_context& context);
	void ideology(association_type, std::string_view value, error_handler& err, int32_t line, national_focus_context& context);
	void loyalty_value(association_type, float value, error_handler& err, int32_t line, national_focus_context& context);
	void any_value(std::string_view label, association_type, float value, error_handler& err, int32_t line, national_focus_context& context);
};

struct focus_group {
	void finish(national_focus_context&) { }
};
struct national_focus_file {
	void finish(scenario_building_context&) { }
};

dcon::trigger_key make_focus_limit(token_generator& gen, error_handler& err, national_focus_context& context);
void make_focus(std::string_view name, token_generator& gen, error_handler& err, national_focus_context& context);
void make_focus_group(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);

struct main_pop_type_file {
	void finish(scenario_building_context&) { }
	void promotion_chance(dcon::value_modifier_key value, error_handler& err, int32_t line, scenario_building_context& context);
	void demotion_chance(dcon::value_modifier_key value, error_handler& err, int32_t line, scenario_building_context& context);
	void migration_chance(dcon::value_modifier_key value, error_handler& err, int32_t line, scenario_building_context& context);
	void colonialmigration_chance(dcon::value_modifier_key value, error_handler& err, int32_t line,
			scenario_building_context& context);
	void emigration_chance(dcon::value_modifier_key value, error_handler& err, int32_t line, scenario_building_context& context);
	void assimilation_chance(dcon::value_modifier_key value, error_handler& err, int32_t line, scenario_building_context& context);
	void conversion_chance(dcon::value_modifier_key value, error_handler& err, int32_t line, scenario_building_context& context);
};

dcon::value_modifier_key make_poptype_pop_chance(token_generator& gen, error_handler& err, scenario_building_context& context);

struct tech_context {
	scenario_building_context& outer_context;
	dcon::technology_id id;
};
struct invention_context {
	scenario_building_context& outer_context;
	dcon::invention_id id;
};

struct unit_modifier_body : public sys::unit_modifier {
	template<typename T>
	void finish(T&) { }
};

struct tech_rgo_goods_output {
	void finish(tech_context&) { }
	void any_value(std::string_view label, association_type, float value, error_handler& err, int32_t line, tech_context& context);
};
struct tech_fac_goods_output {
	void finish(tech_context&) { }
	void any_value(std::string_view label, association_type, float value, error_handler& err, int32_t line, tech_context& context);
};

struct tech_rgo_size {
	void finish(tech_context&) { }
	void finish(invention_context&) { }
	void any_value(std::string_view label, association_type, float value, error_handler& err, int32_t line, tech_context& context);
	void any_value(std::string_view label, association_type, float value, error_handler& err, int32_t line, invention_context& context);
};

struct technology_contents : public modifier_base {
	void any_group(std::string_view label, unit_modifier_body const& value, error_handler& err, int32_t line,
			tech_context& context);
	void any_value(std::string_view name, association_type, int32_t value, error_handler& err, int32_t line, tech_context& context);
	void ai_chance(dcon::value_modifier_key value, error_handler& err, int32_t line, tech_context& context);
	void year(association_type, int32_t value, error_handler& err, int32_t line, tech_context& context);
	void cost(association_type, int32_t value, error_handler& err, int32_t line, tech_context& context);
	void area(association_type, std::string_view value, error_handler& err, int32_t line, tech_context& context);
	void colonial_points(association_type, int32_t value, error_handler& err, int32_t line, tech_context& context);
	void activate_unit(association_type, std::string_view value, error_handler& err, int32_t line, tech_context& context);
	void activate_building(association_type, std::string_view value, error_handler& err, int32_t line, tech_context& context);
	void plurality(association_type, float value, error_handler& err, int32_t line, tech_context& context);

	tech_rgo_goods_output rgo_goods_output;
	tech_rgo_size rgo_size;
	tech_fac_goods_output factory_goods_output;
};

dcon::value_modifier_key make_ai_chance(token_generator& gen, error_handler& err, tech_context& context);
void read_pending_technology(dcon::technology_id id, token_generator& gen, error_handler& err, scenario_building_context& context);



struct inv_rgo_goods_output {
	void finish(invention_context&) { }
	void any_value(std::string_view label, association_type, float value, error_handler& err, int32_t line,
			invention_context& context);
};
struct inv_fac_goods_output {
	void finish(invention_context&) { }
	void any_value(std::string_view label, association_type, float value, error_handler& err, int32_t line,
			invention_context& context);
};
struct inv_fac_goods_throughput {
	void finish(invention_context&) { }
	void any_value(std::string_view label, association_type, float value, error_handler& err, int32_t line,
			invention_context& context);
};
struct inv_rebel_org_gain {
	void finish(invention_context&) { }
	dcon::rebel_type_id faction_;
	float value = 0.0f;
	void faction(association_type, std::string_view v, error_handler& err, int32_t line, invention_context& context);
};
struct inv_effect : public modifier_base {
	void any_group(std::string_view label, unit_modifier_body const& value, error_handler& err, int32_t line,
			invention_context& context);
	void any_value(std::string_view name, association_type, int32_t value, error_handler& err, int32_t line, invention_context& context);
	void activate_unit(association_type, std::string_view value, error_handler& err, int32_t line, invention_context& context);
	void activate_building(association_type, std::string_view value, error_handler& err, int32_t line, invention_context& context);
	inv_rgo_goods_output rgo_goods_output;
	inv_fac_goods_throughput factory_goods_throughput;
	inv_fac_goods_output factory_goods_output;
	tech_rgo_size rgo_size;

	void shared_prestige(association_type, float value, error_handler& err, int32_t line, invention_context& context);
	void plurality(association_type, float value, error_handler& err, int32_t line, invention_context& context);
	void colonial_points(association_type, int32_t value, error_handler& err, int32_t line, invention_context& context);
	void enable_crime(association_type, std::string_view value, error_handler& err, int32_t line, invention_context& context);
	void gas_attack(association_type, bool value, error_handler& err, int32_t line, invention_context& context);
	void gas_defence(association_type, bool value, error_handler& err, int32_t line, invention_context& context);
	void rebel_org_gain(inv_rebel_org_gain const& value, error_handler& err, int32_t line, invention_context& context);
};

struct invention_contents : public modifier_base {
	void limit(dcon::trigger_key value, error_handler& err, int32_t line, invention_context& context);
	void chance(dcon::value_modifier_key value, error_handler& err, int32_t line, invention_context& context);
	void effect(inv_effect const& value, error_handler& err, int32_t line, invention_context& context);
	void shared_prestige(association_type, float value, error_handler& err, int32_t line, invention_context& context);
};

dcon::value_modifier_key make_inv_chance(token_generator& gen, error_handler& err, invention_context& context);
dcon::trigger_key make_inv_limit(token_generator& gen, error_handler& err, invention_context& context);
void read_pending_invention(dcon::invention_id id, token_generator& gen, error_handler& err, scenario_building_context& context);

struct s_on_yearly_pulse {
	void finish(scenario_building_context&) { }
	void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line,
			scenario_building_context& context);
};

struct s_on_quarterly_pulse {
	void finish(scenario_building_context&) { }
	void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line,
			scenario_building_context& context);
};

struct s_on_battle_won {
	void finish(scenario_building_context&) { }
	void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line,
			scenario_building_context& context);
};

struct s_on_battle_lost {
	void finish(scenario_building_context&) { }
	void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line,
			scenario_building_context& context);
};

struct s_on_surrender {
	void finish(scenario_building_context&) { }
	void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line,
			scenario_building_context& context);
};

struct s_on_new_great_nation {
	void finish(scenario_building_context&) { }
	void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line,
			scenario_building_context& context);
};

struct s_on_lost_great_nation {
	void finish(scenario_building_context&) { }
	void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line,
			scenario_building_context& context);
};

struct s_on_election_tick {
	void finish(scenario_building_context&) { }
	void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line,
			scenario_building_context& context);
};

struct s_on_colony_to_state {
	void finish(scenario_building_context&) { }
	void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line,
			scenario_building_context& context);
};

struct s_on_state_conquest {
	void finish(scenario_building_context&) { }
	void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line,
			scenario_building_context& context);
};

struct s_on_colony_to_state_free_slaves {
	void finish(scenario_building_context&) { }
	void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line,
			scenario_building_context& context);
};

struct s_on_debtor_default {
	void finish(scenario_building_context&) { }
	void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line,
			scenario_building_context& context);
};

struct s_on_debtor_default_small {
	void finish(scenario_building_context&) { }
	void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line,
			scenario_building_context& context);
};

struct s_on_debtor_default_second {
	void finish(scenario_building_context&) { }
	void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line,
			scenario_building_context& context);
};

struct s_on_civilize {
	void finish(scenario_building_context&) { }
	void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line,
			scenario_building_context& context);
};

struct s_on_crisis_declare_interest {
	void finish(scenario_building_context&) { }
	void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line,
			scenario_building_context& context);
};

struct s_on_my_factories_nationalized {
	void finish(scenario_building_context&) { }
	void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line,
			scenario_building_context& context);
};
struct on_action_file {
	void finish(scenario_building_context&) { }
	s_on_yearly_pulse on_yearly_pulse;
	s_on_quarterly_pulse on_quarterly_pulse;
	s_on_battle_won on_battle_won;
	s_on_battle_lost on_battle_lost;
	s_on_surrender on_surrender;
	s_on_new_great_nation on_new_great_nation;
	s_on_lost_great_nation on_lost_great_nation;
	s_on_election_tick on_election_tick;
	s_on_colony_to_state on_colony_to_state;
	s_on_state_conquest on_state_conquest;
	s_on_colony_to_state_free_slaves on_colony_to_state_free_slaves;
	s_on_debtor_default on_debtor_default;
	s_on_debtor_default_small on_debtor_default_small;
	s_on_debtor_default_second on_debtor_default_second;
	s_on_civilize on_civilize;
	s_on_my_factories_nationalized on_my_factories_nationalized;
	s_on_crisis_declare_interest on_crisis_declare_interest;
};

struct rebel_context {
	scenario_building_context& outer_context;
	dcon::rebel_type_id id;
};

struct rebel_gov_list {
	void finish(rebel_context&) { }
	void any_value(std::string_view from_gov, association_type, std::string_view to_gov, error_handler& err, int32_t line,
			rebel_context& context);
};

struct rebel_body {
	void finish(rebel_context&) { }
	void icon(association_type, int32_t value, error_handler& err, int32_t line, rebel_context& context);
	void break_alliance_on_win(association_type, bool value, error_handler& err, int32_t line, rebel_context& context);
	rebel_gov_list government;
	void area(association_type, std::string_view value, error_handler& err, int32_t line, rebel_context& context);
	void defection(association_type, std::string_view value, error_handler& err, int32_t line, rebel_context& context);
	void independence(association_type, std::string_view value, error_handler& err, int32_t line, rebel_context& context);
	void defect_delay(association_type, int32_t value, error_handler& err, int32_t line, rebel_context& context);
	void ideology(association_type, std::string_view value, error_handler& err, int32_t line, rebel_context& context);
	void allow_all_cultures(association_type, bool value, error_handler& err, int32_t line, rebel_context& context);
	void allow_all_ideologies(association_type, bool value, error_handler& err, int32_t line, rebel_context& context);
	void allow_all_culture_groups(association_type, bool value, error_handler& err, int32_t line, rebel_context& context);
	void occupation_mult(association_type, float value, error_handler& err, int32_t line, rebel_context& context);
	void will_rise(dcon::value_modifier_key value, error_handler& err, int32_t line, rebel_context& context);
	void spawn_chance(dcon::value_modifier_key value, error_handler& err, int32_t line, rebel_context& context);
	void movement_evaluation(dcon::value_modifier_key value, error_handler& err, int32_t line, rebel_context& context);
	void siege_won_trigger(dcon::trigger_key value, error_handler& err, int32_t line, rebel_context& context);
	void demands_enforced_trigger(dcon::trigger_key value, error_handler& err, int32_t line, rebel_context& context);
	void siege_won_effect(dcon::effect_key value, error_handler& err, int32_t line, rebel_context& context);
	void demands_enforced_effect(dcon::effect_key value, error_handler& err, int32_t line, rebel_context& context);
};

dcon::value_modifier_key make_reb_will_rise(token_generator& gen, error_handler& err, rebel_context& context);
dcon::value_modifier_key make_reb_spawn_chance(token_generator& gen, error_handler& err, rebel_context& context);
dcon::value_modifier_key make_reb_movement_eval(token_generator& gen, error_handler& err, rebel_context& context);
dcon::trigger_key make_reb_s_won_trigger(token_generator& gen, error_handler& err, rebel_context& context);
dcon::trigger_key make_reb_enforced_trigger(token_generator& gen, error_handler& err, rebel_context& context);
dcon::effect_key make_reb_s_won_effect(token_generator& gen, error_handler& err, rebel_context& context);
dcon::effect_key make_reb_enforce_effect(token_generator& gen, error_handler& err, rebel_context& context);
void read_pending_rebel_type(dcon::rebel_type_id id, token_generator& gen, error_handler& err,
		scenario_building_context& context);

struct decision_context {
	scenario_building_context& outer_context;
	dcon::decision_id id;
};

struct decision {
	void finish(decision_context&) { }
	void potential(dcon::trigger_key value, error_handler& err, int32_t line, decision_context& context);
	void allow(dcon::trigger_key value, error_handler& err, int32_t line, decision_context& context);
	void effect(dcon::effect_key value, error_handler& err, int32_t line, decision_context& context);
	void ai_will_do(dcon::value_modifier_key value, error_handler& err, int32_t line, decision_context& context);
	void picture(association_type, std::string_view value, error_handler& err, int32_t line, decision_context& context);
};
struct decision_list {
	void finish(scenario_building_context&) { }
};
struct decision_file {
	void finish(scenario_building_context&) { }
	decision_list political_decisions;
};

dcon::trigger_key make_decision_trigger(token_generator& gen, error_handler& err, decision_context& context);
dcon::effect_key make_decision_effect(token_generator& gen, error_handler& err, decision_context& context);
dcon::value_modifier_key make_decision_ai_choice(token_generator& gen, error_handler& err, decision_context& context);
void make_decision(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);

struct event_file {
	void finish(scenario_building_context&) { }
};

void scan_province_event(token_generator& gen, error_handler& err, scenario_building_context& context);
void scan_country_event(token_generator& gen, error_handler& err, scenario_building_context& context);

struct scan_event {
	bool is_triggered_only = false;
	int32_t id = 0;
	void finish(scenario_building_context&) { }
};

struct event_building_context {
	scenario_building_context& outer_context;

	trigger::slot_contents main_slot = trigger::slot_contents::empty;
	trigger::slot_contents this_slot = trigger::slot_contents::empty;
	trigger::slot_contents from_slot = trigger::slot_contents::empty;
};

struct generic_event {
	int32_t id = 0;
	dcon::trigger_key trigger;
	dcon::value_modifier_key mean_time_to_happen;
	std::array<sys::event_option, sys::max_event_options> options;
	int32_t last_option_added = 0;
	dcon::effect_key immediate_;
	bool major = false;
	bool fire_only_once = false;
	dcon::gfx_object_id picture_;
	dcon::text_sequence_id title_;
	dcon::text_sequence_id desc_;

	void title(association_type, std::string_view value, error_handler& err, int32_t line, event_building_context& context);
	void desc(association_type, std::string_view value, error_handler& err, int32_t line, event_building_context& context);
	void option(sys::event_option const& value, error_handler& err, int32_t line, event_building_context& context);
	void immediate(dcon::effect_key value, error_handler& err, int32_t line, event_building_context& context) {
		if(!bool(immediate_))
			immediate_ = value;
	}
	void picture(association_type, std::string_view value, error_handler& err, int32_t line, event_building_context& context);
	void finish(event_building_context& context) {
		if(!picture_) {
			error_handler err("");
			picture(association_type::eq, "", err, 0, context);
		}
	}
};

dcon::trigger_key make_event_trigger(token_generator& gen, error_handler& err, event_building_context& context);
dcon::effect_key make_immediate_effect(token_generator& gen, error_handler& err, event_building_context& context);
dcon::value_modifier_key make_event_mtth(token_generator& gen, error_handler& err, event_building_context& context);
sys::event_option make_event_option(token_generator& gen, error_handler& err, event_building_context& context);
void commit_pending_events(error_handler& err, scenario_building_context& context);

struct oob_file_context {
	scenario_building_context& outer_context;
	dcon::nation_id nation_for;
};
struct oob_file_relation_context {
	scenario_building_context& outer_context;
	dcon::nation_id nation_for;
	dcon::nation_id nation_with;
};
struct oob_file_regiment_context {
	scenario_building_context& outer_context;
	dcon::regiment_id id;
};
struct oob_file_ship_context {
	scenario_building_context& outer_context;
	dcon::ship_id id;
};
struct oob_file_army_context {
	scenario_building_context& outer_context;
	dcon::army_id id;
	dcon::nation_id nation_for;
};
struct oob_file_navy_context {
	scenario_building_context& outer_context;
	dcon::navy_id id;
	dcon::nation_id nation_for;
};
struct oob_leader {
	void finish(oob_file_context&) { }
	dcon::unit_name_id name_;
	sys::date date_;
	bool is_general = true;
	dcon::leader_trait_id personality_;
	dcon::leader_trait_id background_;
	float prestige = 0.0f;

	void name(association_type, std::string_view value, error_handler& err, int32_t line, oob_file_context& context);
	void date(association_type, sys::year_month_day value, error_handler& err, int32_t line, oob_file_context& context);
	void type(association_type, std::string_view value, error_handler& err, int32_t line, oob_file_context& context) {
		if(is_fixed_token_ci(value.data(), value.data() + value.length(), "sea"))
			is_general = false;
		else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "land"))
			is_general = true;
		else
			err.accumulated_errors += "Leader of type neither land nor sea (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
	void personality(association_type, std::string_view value, error_handler& err, int32_t line, oob_file_context& context) {
		if(auto it = context.outer_context.map_of_leader_traits.find(std::string(value));
				it != context.outer_context.map_of_leader_traits.end()) {
			personality_ = it->second;
		} else {
			err.accumulated_errors +=
					"Invalid leader trait " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void background(association_type, std::string_view value, error_handler& err, int32_t line, oob_file_context& context) {
		if(auto it = context.outer_context.map_of_leader_traits.find(std::string(value));
				it != context.outer_context.map_of_leader_traits.end()) {
			background_ = it->second;
		} else {
			err.accumulated_errors +=
					"Invalid leader trait " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
};
struct oob_army {
	void finish(oob_file_army_context&) { }
	void name(association_type, std::string_view value, error_handler& err, int32_t line, oob_file_army_context& context);
	void location(association_type, int32_t value, error_handler& err, int32_t line, oob_file_army_context& context);
	void leader(oob_leader const& value, error_handler& err, int32_t line, oob_file_army_context& context);
};
struct oob_navy {
	void finish(oob_file_navy_context&) { }
	void name(association_type, std::string_view value, error_handler& err, int32_t line, oob_file_navy_context& context);
	void location(association_type, int32_t value, error_handler& err, int32_t line, oob_file_navy_context& context);
};
struct oob_ship {
	void finish(oob_file_ship_context&) { }
	void name(association_type, std::string_view value, error_handler& err, int32_t line, oob_file_ship_context& context);
	void type(association_type, std::string_view value, error_handler& err, int32_t line, oob_file_ship_context& context);
};

struct oob_regiment {
	void finish(oob_file_regiment_context&) { }
	void name(association_type, std::string_view value, error_handler& err, int32_t line, oob_file_regiment_context& context);
	void type(association_type, std::string_view value, error_handler& err, int32_t line, oob_file_regiment_context& context);
	void home(association_type, int32_t value, error_handler& err, int32_t line, oob_file_regiment_context& context);
};

struct oob_relationship {
	void finish(oob_file_relation_context&) { }
	void value(association_type, int32_t v, error_handler& err, int32_t line, oob_file_relation_context& context);
	void level(association_type, int32_t v, error_handler& err, int32_t line, oob_file_relation_context& context);
	void influence_value(association_type, float v, error_handler& err, int32_t line, oob_file_relation_context& context);
	void truce_until(association_type, sys::year_month_day v, error_handler& err, int32_t line, oob_file_relation_context& context);
};
struct oob_file {
	void finish(oob_file_context&) { }
	void leader(oob_leader const& value, error_handler& err, int32_t line, oob_file_context& context);
};

oob_leader make_army_leader(token_generator& gen, error_handler& err, oob_file_army_context& context);
void make_oob_relationship(std::string_view tag, token_generator& gen, error_handler& err, oob_file_context& context);
void make_oob_army(token_generator& gen, error_handler& err, oob_file_context& context);
void make_oob_navy(token_generator& gen, error_handler& err, oob_file_context& context);
void make_oob_regiment(token_generator& gen, error_handler& err, oob_file_army_context& context);
void make_oob_ship(token_generator& gen, error_handler& err, oob_file_navy_context& context);

struct production_type;

struct production_context {
	scenario_building_context& outer_context;
	ankerl::unordered_dense::map<std::string, production_type> templates;
	bool found_worker_types = false;

	production_context(scenario_building_context& outer_context) : outer_context(outer_context) { }
};

struct production_types_file {
	void finish(production_context&) { }
};
struct production_employee {
	void finish(production_context&) { }
	float amount = 0.0f;
	dcon::pop_type_id type;

	void poptype(association_type, std::string_view v, error_handler& err, int32_t line, production_context& context);
};

struct production_employee_set {
	std::vector<production_employee> employees;
	void finish(production_context&) { }
	void free_group(production_employee const& value, error_handler& err, int32_t line, production_context& context) {
		employees.push_back(value);
	}
};

struct production_bonus {
	dcon::trigger_key trigger;
	float value = 0.0f;

	void finish(production_context&) { }
};

enum class production_type_enum { none = 0, factory, rgo, artisan };
struct production_type {
	commodity_array efficiency;
	commodity_array input_goods;
	production_employee owner;
	production_employee_set employees;
	int32_t workforce = 0;
	bool farm = false;
	bool mine = false;
	bool is_coastal = false;
	float value = 0.0f;

	std::vector<production_bonus> bonuses;
	dcon::commodity_id output_goods_;
	production_type_enum type_ = production_type_enum::none;

	void output_goods(association_type, std::string_view v, error_handler& err, int32_t line, production_context& context) {
		if(auto it = context.outer_context.map_of_commodity_names.find(std::string(v));
				it != context.outer_context.map_of_commodity_names.end()) {
			output_goods_ = it->second;
		} else {
			err.accumulated_errors +=
					"Invalid commodity name " + std::string(v) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void bonus(production_bonus const& v, error_handler& err, int32_t line, production_context& context) {
		bonuses.push_back(v);
	}
	void type(association_type, std::string_view v, error_handler& err, int32_t line, production_context& context) {
		if(is_fixed_token_ci(v.data(), v.data() + v.length(), "factory"))
			type_ = production_type_enum::factory;
		else if(is_fixed_token_ci(v.data(), v.data() + v.length(), "rgo"))
			type_ = production_type_enum::rgo;
		else if(is_fixed_token_ci(v.data(), v.data() + v.length(), "artisan"))
			type_ = production_type_enum::artisan;
		else
			err.accumulated_errors +=
					"Invalid production type " + std::string(v) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
	}
	void as_template(association_type, std::string_view v, error_handler& err, int32_t line, production_context& context) {
		if(auto it = context.templates.find(std::string(v)); it != context.templates.end()) {
			*this = it->second;
		} else {
			err.accumulated_errors +=
					"Invalid production template " + std::string(v) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
		}
	}
	void finish(production_context&) { }
};

commodity_array make_prod_commodity_array(token_generator& gen, error_handler& err, production_context& context);

dcon::trigger_key make_production_bonus_trigger(token_generator& gen, error_handler& err, production_context& context);
void make_production_type(std::string_view name, token_generator& gen, error_handler& err, production_context& context);

struct alliance {
	dcon::nation_id first_;
	dcon::nation_id second_;

	void finish(scenario_building_context&) { }
	void first(association_type, std::string_view tag, error_handler& err, int32_t line, scenario_building_context& context);
	void second(association_type, std::string_view tag, error_handler& err, int32_t line, scenario_building_context& context);
};
struct vassal_description {
	dcon::nation_id first_;
	dcon::nation_id second_;
	bool invalid = false;
	void finish(scenario_building_context&) { }
	void first(association_type, std::string_view tag, error_handler& err, int32_t line, scenario_building_context& context);
	void second(association_type, std::string_view tag, error_handler& err, int32_t line, scenario_building_context& context);
	void start_date(association_type, sys::year_month_day ymd, error_handler& err, int32_t line, scenario_building_context& context);
};

struct alliance_file {
	void finish(scenario_building_context&) { }
};
struct union_file {
	void finish(scenario_building_context&) { }
};
struct puppets_file {
	void finish(scenario_building_context&) { }
};

void make_alliance(token_generator& gen, error_handler& err, scenario_building_context& context);
void make_vassal(token_generator& gen, error_handler& err, scenario_building_context& context);
void make_substate(token_generator& gen, error_handler& err, scenario_building_context& context);

struct country_history_context {
	scenario_building_context& outer_context;
	dcon::national_identity_id nat_ident;
	dcon::nation_id holder_id;
	std::vector<std::pair<dcon::nation_id, dcon::decision_id>>& pending_decisions;
};

struct govt_flag_block {
	void finish(country_history_context&) { }

	::culture::flag_type flag_ = ::culture::flag_type::default_flag;
	dcon::government_type_id government_;

	void flag(association_type, std::string_view value, error_handler& err, int32_t line, country_history_context& context);
	void government(association_type, std::string_view value, error_handler& err, int32_t line, country_history_context& context) {
		if(auto it = context.outer_context.map_of_governments.find(std::string(value));
				it != context.outer_context.map_of_governments.end()) {
			government_ = it->second;
		} else {
			err.accumulated_errors += "invalid government type " + std::string(value) + " encountered  (" + err.file_name + " line " +
																std::to_string(line) + ")\n";
		}
	}
};
struct upper_house_block {
	void finish(country_history_context&) { }
	void any_value(std::string_view value, association_type, float v, error_handler& err, int32_t line,
			country_history_context& context);
};
struct foreign_investment_block {
	void finish(country_history_context&) { }
	void any_value(std::string_view tag, association_type, float v, error_handler& err, int32_t line,
			country_history_context& context);
};

struct country_history_file {
	void finish(country_history_context&) { }
	void set_country_flag(association_type, std::string_view value, error_handler& err, int32_t line, country_history_context& context);
	void set_global_flag(association_type, std::string_view value, error_handler& err, int32_t line, country_history_context& context);
	void colonial_points(association_type, int32_t value, error_handler& err, int32_t line, country_history_context& context);
	void capital(association_type, int32_t value, error_handler& err, int32_t line, country_history_context& context);
	void any_value(std::string_view label, association_type, std::string_view value, error_handler& err, int32_t line,
			country_history_context& context);
	void primary_culture(association_type, std::string_view value, error_handler& err, int32_t line,
			country_history_context& context);
	void culture(association_type, std::string_view value, error_handler& err, int32_t line, country_history_context& context);
	void religion(association_type, std::string_view value, error_handler& err, int32_t line, country_history_context& context);
	void government(association_type, std::string_view value, error_handler& err, int32_t line, country_history_context& context);
	void plurality(association_type, float value, error_handler& err, int32_t line, country_history_context& context);
	void prestige(association_type, float value, error_handler& err, int32_t line, country_history_context& context);
	void nationalvalue(association_type, std::string_view value, error_handler& err, int32_t line,
			country_history_context& context);
	void schools(association_type, std::string_view value, error_handler& err, int32_t line, country_history_context& context);

	foreign_investment_block foreign_investment;
	upper_house_block upper_house;

	void civilized(association_type, bool value, error_handler& err, int32_t line, country_history_context& context);
	void is_releasable_vassal(association_type, bool value, error_handler& err, int32_t line, country_history_context& context);
	void literacy(association_type, float value, error_handler& err, int32_t line, country_history_context& context);
	void non_state_culture_literacy(association_type, float value, error_handler& err, int32_t line,
			country_history_context& context);
	void consciousness(association_type, float value, error_handler& err, int32_t line, country_history_context& context);
	void nonstate_consciousness(association_type, float value, error_handler& err, int32_t line, country_history_context& context);
	void govt_flag(govt_flag_block const& value, error_handler& err, int32_t line, country_history_context& context);
	void ruling_party(association_type, std::string_view value, error_handler& err, int32_t line, country_history_context& context);
	void decision(association_type, std::string_view value, error_handler& err, int32_t line, country_history_context& context);
};

void enter_country_file_dated_block(std::string_view label, token_generator& gen, error_handler& err,
		country_history_context& context);

struct war_history_context;

struct history_war_goal {
	dcon::nation_id actor_;
	dcon::nation_id receiver_;
	dcon::cb_type_id casus_belli_;
	dcon::nation_id secondary_;
	dcon::province_id state_province_id_;

	void receiver(association_type, std::string_view tag, error_handler& err, int32_t line, war_history_context& context);
	void actor(association_type, std::string_view tag, error_handler& err, int32_t line, war_history_context& context);
	void country(association_type, std::string_view tag, error_handler& err, int32_t line, war_history_context& context);
	void casus_belli(association_type, std::string_view value, error_handler& err, int32_t line, war_history_context& context);
	void state_province_id(association_type t, int32_t value, error_handler& err, int32_t line, war_history_context& context);

	void finish(war_history_context&) { }
};

struct war_history_context {
	scenario_building_context& outer_context;

	std::vector<history_war_goal> wargoals;
	std::vector<dcon::nation_id> attackers;
	std::vector<dcon::nation_id> defenders;
	std::string name;
	bool great_war = false;

	war_history_context(scenario_building_context& outer_context) : outer_context(outer_context) { }
};

struct war_block {
	void add_attacker(association_type, std::string_view tag, error_handler& err, int32_t line, war_history_context& context);
	void add_defender(association_type, std::string_view tag, error_handler& err, int32_t line, war_history_context& context);
	void rem_attacker(association_type, std::string_view tag, error_handler& err, int32_t line, war_history_context& context);
	void rem_defender(association_type, std::string_view tag, error_handler& err, int32_t line, war_history_context& context);
	void world_war(association_type, bool v, error_handler& err, int32_t line, war_history_context& context);
	void war_goal(history_war_goal const& value, error_handler& err, int32_t line, war_history_context& context) {
		context.wargoals.push_back(value);
	}

	void finish(war_history_context&) { }
};

struct war_history_file {
	void name(association_type, std::string_view name, error_handler& err, int32_t line, war_history_context& context);
	void finish(war_history_context&);
};

void enter_war_dated_block(std::string_view label, token_generator& gen, error_handler& err, war_history_context& context);

struct mod_file_context {
	
};

struct dependencies_list {
	std::vector<std::string> names;
	template<typename T>
	void free_value(std::string_view value, error_handler& err, int32_t line, T& context) {
		names.push_back(std::string(value));
	}
	template<typename T>
	void finish(T& context) { }
};

struct mod_file {
	std::string name_;
	std::string path_;
	std::string user_dir_;
	std::vector<std::string> replace_paths;
	std::vector<std::string> dependent_mods;

	bool mod_selected = false;

	void name(association_type, std::string_view value, error_handler& err, int32_t line, mod_file_context& context);
	void path(association_type, std::string_view value, error_handler& err, int32_t line, mod_file_context& context);
	void user_dir(association_type, std::string_view value, error_handler& err, int32_t line, mod_file_context& context);
	void replace_path(association_type, std::string_view value, error_handler& err, int32_t line, mod_file_context& context);
	void dependencies(dependencies_list const& value, error_handler& err, int32_t line, mod_file_context& context) {
		for(auto& s : value.names)
			dependent_mods.push_back(s);
	}
	void finish(mod_file_context& context) { }
	void add_to_file_system(simple_fs::file_system& fs);
};

void make_leader_images(scenario_building_context& outer_context);

} // namespace parsers

#include "trigger_parsing.hpp"
#include "effect_parsing.hpp"
#include "parser_defs_generated.hpp"
