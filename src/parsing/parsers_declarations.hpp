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

namespace parsers {

	//
	// structures and functions for parsing .gfx files
	//

	struct building_gfx_context {
		sys::state& full_state;
		ui::definitions& ui_defs;
		ankerl::unordered_dense::map<std::string, dcon::gfx_object_id> map_of_names;
		ankerl::unordered_dense::map<std::string, dcon::texture_id> map_of_texture_names;
		building_gfx_context(sys::state& full_state, ui::definitions& ui_defs) : full_state(full_state), ui_defs(ui_defs) {

		}
	};

	struct gfx_xy_pair {
		int32_t x = 0;
		int32_t y = 0;

		void finish(building_gfx_context& context) { }
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

		void clicksound(parsers::association_type, std::string_view, parsers::error_handler& err, int32_t line, building_gfx_context& context) {
			clicksound_set = true;
		}

		void finish(building_gfx_context& context) {
		}
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

		void finish(building_gfx_context const& context) {
		}
	};

	void gfx_add_obj(parsers::token_generator& gen, parsers::error_handler& err, building_gfx_context& context);

	struct gfx_files {
		void finish(building_gfx_context& context) {
		}
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
		void finish(building_gfx_context& context) {
		}
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
		void maxwidth(association_type, int32_t v, error_handler& err, int32_t line, building_gfx_context& context);
		void maxheight(association_type, int32_t v, error_handler& err, int32_t line, building_gfx_context& context);
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
		void finish(building_gfx_context& context) {
		}
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
		void finish(building_gfx_context& context) {
		}
	};

	struct pending_ideology_content {
		token_generator generator_state;
		dcon::ideology_id id;
	};
	struct pending_option_content {
		token_generator generator_state;
		dcon::issue_option_id id;
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
		dcon::national_event_id id;
		trigger::slot_contents main_slot;
		trigger::slot_contents this_slot;
		trigger::slot_contents from_slot;
	};
	struct pending_prov_event {
		dcon::provincial_event_id id;
		trigger::slot_contents main_slot;
		trigger::slot_contents this_slot;
		trigger::slot_contents from_slot;
	};
	struct scenario_building_context {
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
		ankerl::unordered_dense::map<std::string, pending_option_content> map_of_options;
		ankerl::unordered_dense::map<std::string, dcon::issue_id> map_of_issues;
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
		ankerl::unordered_dense::map<int32_t, pending_nat_event> map_of_national_events;
		ankerl::unordered_dense::map<int32_t, pending_prov_event> map_of_provincial_events;

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
		
		scenario_building_context(sys::state& state) : state(state) { }

		dcon::national_variable_id get_national_variable(std::string const& name);
		dcon::national_flag_id get_national_flag(std::string const& name);
		dcon::global_flag_id get_global_flag(std::string const& name);
	};

	struct national_identity_file {
		void any_value(std::string_view tag, association_type, std::string_view txt, error_handler& err, int32_t line, scenario_building_context& context);
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
		void icon(association_type, int32_t v, error_handler& err, int32_t line, religion_context& context) {
			context.outer_context.state.world.religion_set_icon(context.id, uint8_t(v));
		}
		void color(color_from_3f v, error_handler& err, int32_t line, religion_context& context) {
			context.outer_context.state.world.religion_set_color(context.id, v.value);
		}
		void pagan(association_type, bool v, error_handler& err, int32_t line, religion_context& context) {
			context.outer_context.state.world.religion_set_is_pagan(context.id, v);
		}
		void finish(religion_context& context) { }
	};

	void make_religion(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);

	struct religion_group {
		void finish(scenario_building_context& context) { }
	};

	struct religion_file {
		void any_group(std::string_view name, religion_group, error_handler& err, int32_t line, scenario_building_context& context) {

		}
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
		void free_value(std::string_view text, error_handler& err, int32_t line, names_context& context) {
			auto new_id = context.outer_context.state.add_to_pool(text);
			if(context.first_names) {
				context.outer_context.state.world.culture_get_first_names(context.id).push_back(new_id);
			} else {
				context.outer_context.state.world.culture_get_last_names(context.id).push_back(new_id);
			}
		}
		void finish(names_context& context) { }
	};

	struct culture {
		void color(color_from_3i v, error_handler& err, int32_t line, culture_context& context) {
			context.outer_context.state.world.culture_set_color(context.id, v.value);
		}
		void radicalism(association_type, int32_t v, error_handler& err, int32_t line, culture_context& context) {
			context.outer_context.state.world.culture_set_radicalism(context.id, int8_t(v));
		}
		void finish(culture_context& context) { }
	};

	struct culture_group {
		void leader(association_type, std::string_view name, error_handler& err, int32_t line, culture_group_context& context) {
			if(name == "european")
				context.outer_context.state.world.culture_group_set_leader(context.id, uint8_t(sys::leader_type::european));
			else if(name == "southamerican")
				context.outer_context.state.world.culture_group_set_leader(context.id, uint8_t(sys::leader_type::southamerican));
			else if(name == "russian")
				context.outer_context.state.world.culture_group_set_leader(context.id, uint8_t(sys::leader_type::russian));
			else if(name == "arab")
				context.outer_context.state.world.culture_group_set_leader(context.id, uint8_t(sys::leader_type::arab));
			else if(name == "asian")
				context.outer_context.state.world.culture_group_set_leader(context.id, uint8_t(sys::leader_type::asian));
			else if(name == "indian")
				context.outer_context.state.world.culture_group_set_leader(context.id, uint8_t(sys::leader_type::indian));
			else if(name == "nativeamerican")
				context.outer_context.state.world.culture_group_set_leader(context.id, uint8_t(sys::leader_type::nativeamerican));
			else if(name == "african")
				context.outer_context.state.world.culture_group_set_leader(context.id, uint8_t(sys::leader_type::african));
			else if(name == "polar_bear")
				context.outer_context.state.world.culture_group_set_leader(context.id, uint8_t(sys::leader_type::polar_bear));
			else {
				err.accumulated_errors += "Unknown leader type " + std::string(name) + " in file " + err.file_name + " line " + std::to_string(line) + "\n";
			}
		}
		void is_overseas(association_type, bool v, error_handler& err, int32_t line, culture_group_context& context) {

			context.outer_context.state.world.culture_group_set_is_overseas(context.id, v);
		}
		void union_tag(association_type, uint32_t v, error_handler& err, int32_t line, culture_group_context& context) {
			auto nat_tag = context.outer_context.map_of_ident_names.find(v);
			if(nat_tag != context.outer_context.map_of_ident_names.end())
				context.outer_context.state.world.force_create_cultural_union_of(nat_tag->second, context.id);
			else
				err.accumulated_errors += "Unknown national tag in file " + err.file_name + " line " + std::to_string(line) + "\n";
		}
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
		void money(association_type, bool v, error_handler& err, int32_t line, good_context& context) {
			if(v) {
				auto money_id = dcon::commodity_id(0);
				context.outer_context.state.world.commodity_set_color(money_id, context.outer_context.state.world.commodity_get_color(context.id));
				context.outer_context.state.world.commodity_set_cost(money_id, context.outer_context.state.world.commodity_get_cost(context.id));

				for(auto& pr : context.outer_context.map_of_commodity_names) {
					if(pr.second == context.id) {
						pr.second = money_id;
						break;
					}
				}
				context.id = money_id;
				context.outer_context.state.world.pop_back_commodity();
			}
		}
		void color(color_from_3i v, error_handler& err, int32_t line, good_context& context) {
			context.outer_context.state.world.commodity_set_color(context.id, v.value);
		}
		void cost(association_type, float v, error_handler& err, int32_t line, good_context& context) {
			context.outer_context.state.world.commodity_set_cost(context.id, v);
		}
		void available_from_start(association_type, bool b, error_handler& err, int32_t line, good_context& context) {
			context.outer_context.state.world.commodity_set_is_available_from_start(context.id, b);
		}

		void finish(good_context& context) { }
	};

	void make_good(std::string_view name, token_generator& gen, error_handler& err, good_group_context& context);
	void make_goods_group(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);

	struct goods_group {
		void finish(good_group_context& context) { }
	};

	struct goods_file {
		void finish(scenario_building_context& context) { }
	};

#define MOD_PROV_FUNCTION(X) template<typename T> \
	void X (association_type, float v, error_handler& err, int32_t line, T& context) { \
		if(next_to_add >= sys::modifier_definition_size) { \
			err.accumulated_errors += "Too many modifier values; " + err.file_name + " line " + std::to_string(line) + "\n"; \
		} else { \
			constructed_definition.offsets[next_to_add] = uint8_t(sys::provincial_mod_offsets:: X ); \
			constructed_definition.values[next_to_add] = v; \
			++next_to_add; \
		} \
	}
#define MOD_NAT_FUNCTION(X) template<typename T> \
	void X (association_type, float v, error_handler& err, int32_t line, T& context) { \
		if(next_to_add >= sys::modifier_definition_size) { \
			err.accumulated_errors += "Too many modifier values; " + err.file_name + " line " + std::to_string(line) + "\n"; \
		} else { \
			constructed_definition.offsets[next_to_add] = uint8_t(sys::national_mod_offsets:: X ); \
			constructed_definition.values[next_to_add] = v; \
			++next_to_add; \
		} \
	}

	struct modifier_base {
		uint8_t icon_index = 0;
		sys::modifier_definition constructed_definition;
		uint32_t next_to_add = 0;
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
		MOD_NAT_FUNCTION(tax_efficiency)
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
		MOD_NAT_FUNCTION(global_population_growth)
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
			if(next_to_add >= sys::modifier_definition_size) {
				err.accumulated_errors += "Too many modifier values; " + err.file_name + " line " + std::to_string(line) + "\n";
			} else {
				constructed_definition.offsets[next_to_add] = uint8_t(sys::provincial_mod_offsets::immigrant_attract);
				constructed_definition.values[next_to_add] = v;
				++next_to_add;
			}
		}
		MOD_PROV_FUNCTION(immigrant_attract)
		MOD_PROV_FUNCTION(immigrant_push)
		MOD_PROV_FUNCTION(local_repair)
		MOD_PROV_FUNCTION(local_ship_build)
		MOD_NAT_FUNCTION(poor_savings_modifier)
		MOD_NAT_FUNCTION(influence_modifier)
		MOD_NAT_FUNCTION(diplomatic_points_modifier)
		MOD_NAT_FUNCTION(mobilisation_size)
		MOD_NAT_FUNCTION(mobilisation_economy_impact)
		template<typename T> \
		void mobilization_size(association_type, float v, error_handler& err, int32_t line, T& context) {
			if(next_to_add >= sys::modifier_definition_size) {
					err.accumulated_errors += "Too many modifier values; " + err.file_name + " line " + std::to_string(line) + "\n";
			} else {
				constructed_definition.offsets[next_to_add] = uint8_t(sys::national_mod_offsets::mobilisation_size);
				constructed_definition.values[next_to_add] = v;
				++next_to_add;
			}
		}
		template<typename T>
		void mobilization_economy_impact(association_type, float v, error_handler& err, int32_t line, T& context) {
			if(next_to_add >= sys::modifier_definition_size) {
				err.accumulated_errors += "Too many modifier values; " + err.file_name + " line " + std::to_string(line) + "\n";
			} else {
				constructed_definition.offsets[next_to_add] = uint8_t(sys::national_mod_offsets::mobilisation_economy_impact);
				constructed_definition.values[next_to_add] = v;
				++next_to_add;
			}
		}
		MOD_NAT_FUNCTION(global_pop_militancy_modifier)
		MOD_NAT_FUNCTION(global_pop_consciousness_modifier)
		MOD_PROV_FUNCTION(movement_cost)
		MOD_PROV_FUNCTION(combat_width)
		MOD_PROV_FUNCTION(min_build_naval_base)
		MOD_PROV_FUNCTION(min_build_railroad)
		MOD_PROV_FUNCTION(min_build_fort)
		MOD_PROV_FUNCTION(attack)
		template<typename T>
		void defender(association_type, float v, error_handler& err, int32_t line, T& context) {
			if(next_to_add >= sys::modifier_definition_size) {
				err.accumulated_errors += "Too many modifier values; " + err.file_name + " line " + std::to_string(line) + "\n";
			} else {
				constructed_definition.offsets[next_to_add] = uint8_t(sys::provincial_mod_offsets::defense);
				constructed_definition.values[next_to_add] = v;
				++next_to_add;
			}
		}
		template<typename T>
		void attacker(association_type, float v, error_handler& err, int32_t line, T& context) {
			if(next_to_add >= sys::modifier_definition_size) {
				err.accumulated_errors += "Too many modifier values; " + err.file_name + " line " + std::to_string(line) + "\n";
			} else {
				constructed_definition.offsets[next_to_add] = uint8_t(sys::provincial_mod_offsets::attack);
				constructed_definition.values[next_to_add] = v;
				++next_to_add;
			}
		}
		template<typename T>
		void defence(association_type, float v, error_handler& err, int32_t line, T& context) {
			if(next_to_add >= sys::modifier_definition_size) {
				err.accumulated_errors += "Too many modifier values; " + err.file_name + " line " + std::to_string(line) + "\n";
			} else {
				constructed_definition.offsets[next_to_add] = uint8_t(sys::provincial_mod_offsets::defense);
				constructed_definition.values[next_to_add] = v;
				++next_to_add;
			}
		}
		MOD_NAT_FUNCTION(core_pop_militancy_modifier)
		MOD_NAT_FUNCTION(core_pop_consciousness_modifier)
		MOD_NAT_FUNCTION(non_accepted_pop_militancy_modifier)
		MOD_NAT_FUNCTION(non_accepted_pop_consciousness_modifier)
		MOD_NAT_FUNCTION(cb_generation_speed_modifier)
		MOD_NAT_FUNCTION(mobilization_impact)
		template<typename T>
		void mobilisation_impact(association_type, float v, error_handler& err, int32_t line, T& context) {
			if(next_to_add >= sys::modifier_definition_size) {
				err.accumulated_errors += "Too many modifier values; " + err.file_name + " line " + std::to_string(line) + "\n";
			} else {
				constructed_definition.offsets[next_to_add] = uint8_t(sys::national_mod_offsets::mobilization_impact);
				constructed_definition.values[next_to_add] = v;
				++next_to_add;
			}
		}
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
		MOD_NAT_FUNCTION(cb_creation_speed)
		MOD_NAT_FUNCTION(education_efficiency)
		MOD_NAT_FUNCTION(diplomatic_points)
		MOD_NAT_FUNCTION(reinforce_rate)
		MOD_NAT_FUNCTION(tax_eff)
		MOD_NAT_FUNCTION(administrative_efficiency)
		MOD_NAT_FUNCTION(influence)
		MOD_NAT_FUNCTION(dig_in_cap)
		MOD_NAT_FUNCTION(morale)
		MOD_NAT_FUNCTION(military_tactics)
		MOD_NAT_FUNCTION(supply_range)
		MOD_NAT_FUNCTION(regular_experience_level)
		MOD_NAT_FUNCTION(increase_research)
		MOD_NAT_FUNCTION(permanent_prestige)
		MOD_NAT_FUNCTION(soldier_to_pop_loss)
		MOD_NAT_FUNCTION(naval_attrition)
		MOD_NAT_FUNCTION(land_attrition)
		MOD_NAT_FUNCTION(pop_growth)
		MOD_NAT_FUNCTION(colonial_life_rating)
		MOD_NAT_FUNCTION(seperatism)
		MOD_NAT_FUNCTION(plurality)
		MOD_NAT_FUNCTION(colonial_prestige)

		template<typename T>
		void finish(T& context) { }

		void convert_to_national_mod() {
			for(uint32_t i = 0; i < this->next_to_add; ++i) {
				if(constructed_definition.offsets[i] == sys::provincial_mod_offsets::poor_life_needs) {
					constructed_definition.offsets[i] = uint8_t(sys::national_mod_offsets::poor_life_needs);
				} else if(constructed_definition.offsets[i] == sys::provincial_mod_offsets::rich_life_needs) {
					constructed_definition.offsets[i] = uint8_t(sys::national_mod_offsets::rich_life_needs);
				} else if(constructed_definition.offsets[i] == sys::provincial_mod_offsets::middle_life_needs) {
					constructed_definition.offsets[i] = uint8_t(sys::national_mod_offsets::middle_life_needs);
				} else if(constructed_definition.offsets[i] == sys::provincial_mod_offsets::poor_everyday_needs) {
					constructed_definition.offsets[i] = uint8_t(sys::national_mod_offsets::poor_everyday_needs);
				} else if(constructed_definition.offsets[i] == sys::provincial_mod_offsets::rich_everyday_needs) {
					constructed_definition.offsets[i] = uint8_t(sys::national_mod_offsets::rich_everyday_needs);
				} else if(constructed_definition.offsets[i] == sys::provincial_mod_offsets::middle_everyday_needs) {
					constructed_definition.offsets[i] = uint8_t(sys::national_mod_offsets::middle_everyday_needs);
				} else if(constructed_definition.offsets[i] == sys::provincial_mod_offsets::poor_luxury_needs) {
					constructed_definition.offsets[i] = uint8_t(sys::national_mod_offsets::poor_luxury_needs);
				} else if(constructed_definition.offsets[i] == sys::provincial_mod_offsets::middle_luxury_needs) {
					constructed_definition.offsets[i] = uint8_t(sys::national_mod_offsets::middle_luxury_needs);
				} else if(constructed_definition.offsets[i] == sys::provincial_mod_offsets::rich_luxury_needs) {
					constructed_definition.offsets[i] = uint8_t(sys::national_mod_offsets::rich_luxury_needs);
				} else if(constructed_definition.offsets[i] == sys::provincial_mod_offsets::goods_demand) {
					constructed_definition.offsets[i] = uint8_t(sys::national_mod_offsets::goods_demand);
				} else if(constructed_definition.offsets[i] == sys::provincial_mod_offsets::assimilation_rate) {
					constructed_definition.offsets[i] = uint8_t(sys::national_mod_offsets::global_assimilation_rate);
				} else if(constructed_definition.offsets[i] == sys::provincial_mod_offsets::farm_rgo_eff) {
					constructed_definition.offsets[i] = uint8_t(sys::national_mod_offsets::farm_rgo_eff);
				} else if(constructed_definition.offsets[i] == sys::provincial_mod_offsets::mine_rgo_eff) {
					constructed_definition.offsets[i] = uint8_t(sys::national_mod_offsets::mine_rgo_eff);
				} else if(constructed_definition.offsets[i] == sys::provincial_mod_offsets::farm_rgo_size) {
					constructed_definition.offsets[i] = uint8_t(sys::national_mod_offsets::farm_rgo_size);
				} else if(constructed_definition.offsets[i] == sys::provincial_mod_offsets::mine_rgo_size) {
					constructed_definition.offsets[i] = uint8_t(sys::national_mod_offsets::mine_rgo_size);
				} else if(constructed_definition.offsets[i] == sys::provincial_mod_offsets::pop_militancy_modifier) {
					constructed_definition.offsets[i] = uint8_t(sys::national_mod_offsets::global_pop_militancy_modifier);
				} else if(constructed_definition.offsets[i] == sys::provincial_mod_offsets::pop_consciousness_modifier) {
					constructed_definition.offsets[i] = uint8_t(sys::national_mod_offsets::global_pop_consciousness_modifier);
				} else if(constructed_definition.offsets[i] == sys::provincial_mod_offsets::rich_income_modifier) {
					constructed_definition.offsets[i] = uint8_t(sys::national_mod_offsets::rich_income_modifier);
				} else if(constructed_definition.offsets[i] == sys::provincial_mod_offsets::middle_income_modifier) {
					constructed_definition.offsets[i] = uint8_t(sys::national_mod_offsets::middle_income_modifier);
				} else if(constructed_definition.offsets[i] == sys::provincial_mod_offsets::poor_income_modifier) {
					constructed_definition.offsets[i] = uint8_t(sys::national_mod_offsets::poor_income_modifier);
				} else if(constructed_definition.offsets[i] == sys::provincial_mod_offsets::supply_limit) {
					constructed_definition.offsets[i] = uint8_t(sys::national_mod_offsets::supply_limit);
				} else if(constructed_definition.offsets[i] == sys::provincial_mod_offsets::combat_width) {
					constructed_definition.offsets[i] = uint8_t(sys::national_mod_offsets::combat_width);
				}
				constructed_definition.offsets[i] += 1;
			}
		}
		void convert_to_province_mod() {
			for(uint32_t i = 0; i < this->next_to_add; ++i) {
				constructed_definition.offsets[i] += 1;
			}
		}
		void convert_to_neutral_mod() {
			for(uint32_t i = 0; i < this->next_to_add; ++i) {
				constructed_definition.offsets[i] -= 1;
			}
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

		void any_value(std::string_view name, association_type, float value, error_handler& err, int32_t line, scenario_building_context& context) {
			auto found_commodity = context.map_of_commodity_names.find(std::string(name));
			if(found_commodity != context.map_of_commodity_names.end()) {
				data.safe_get(found_commodity->second) = value;
			} else {
				err.accumulated_errors += "Unknown commodity " + std::string(name) + " in file " + err.file_name + " line " + std::to_string(line) + "\n";
			}
		}

		void finish(scenario_building_context& context) {
			data.resize(context.state.world.commodity_size());
		}
	};

	enum class building_type {
		factory, naval_base, fort, railroad
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
		building_type stored_type = building_type::factory;

		void type(association_type, std::string_view value, error_handler& err, int32_t line, scenario_building_context& context) {
			if(is_fixed_token_ci(value.data(), value.data() + value.length(), "factory")) {
				stored_type = building_type::factory;
			} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "fort")) {
				stored_type = building_type::fort;
			} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "naval_base")) {
				stored_type = building_type::naval_base;
			} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "infrastructure")) {
				stored_type = building_type::railroad;
			} else {
				err.accumulated_errors += "Unknown building type " + std::string(value) + " in file " + err.file_name + " line " + std::to_string(line) + "\n";
			}
		}
		

		void finish(scenario_building_context& context) { }
	};

	struct building_file {
		void result(std::string_view name, building_definition&& res, error_handler& err, int32_t line, scenario_building_context& context);
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
	struct issue_group_context {
		scenario_building_context& outer_context;
		::culture::issue_category issue_cat = ::culture::issue_category::party;

		issue_group_context(scenario_building_context& outer_context, ::culture::issue_category issue_cat) : outer_context(outer_context), issue_cat(issue_cat) { }
	};
	struct issue {
		void next_step_only(association_type, bool value, error_handler& err, int32_t line, issue_context& context) {
			context.outer_context.state.world.issue_set_is_next_step_only(context.id, value);
		}
		void administrative(association_type, bool value, error_handler& err, int32_t line, issue_context& context) {
			context.outer_context.state.world.issue_set_is_administrative(context.id, value);
		}
		void finish(issue_context&) { }
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

	struct government_type_context {
		scenario_building_context& outer_context;
		dcon::government_type_id id;
	};
	struct government_type {
		void election(association_type, bool value, error_handler& err, int32_t line, government_type_context& context) {
			context.outer_context.state.culture_definitions.governments[context.id].has_elections = value;
		}
		void appoint_ruling_party(association_type, bool value, error_handler& err, int32_t line, government_type_context& context) {
			context.outer_context.state.culture_definitions.governments[context.id].can_appoint_ruling_party = value;
		}
		void duration(association_type, int32_t value, error_handler& err, int32_t line, government_type_context& context) {
			context.outer_context.state.culture_definitions.governments[context.id].duration = int8_t(value);
		}
		void flagtype(association_type, std::string_view value, error_handler& err, int32_t line, government_type_context& context) {
			if(is_fixed_token_ci(value.data(), value.data() + value.length(), "communist"))
				context.outer_context.state.culture_definitions.governments[context.id].flag = ::culture::flag_type::communist;
			else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "fascist"))
				context.outer_context.state.culture_definitions.governments[context.id].flag = ::culture::flag_type::fascist;
			else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "monarchy"))
				context.outer_context.state.culture_definitions.governments[context.id].flag = ::culture::flag_type::monarchy;
			else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "republic"))
				context.outer_context.state.culture_definitions.governments[context.id].flag = ::culture::flag_type::republic;
			else {
				err.accumulated_errors += "Unknown flag type " + std::string(value) + " in file " + err.file_name + " line " + std::to_string(line) + "\n";
			}
		}
		void any_value(std::string_view text, association_type, bool value, error_handler& err, int32_t line, government_type_context& context) {
			if(value) {
				auto found_ideology = context.outer_context.map_of_ideologies.find(std::string(text));
				if(found_ideology != context.outer_context.map_of_ideologies.end()) {
					context.outer_context.state.culture_definitions.governments[context.id].ideologies_allowed |= ::culture::to_bits(found_ideology->second.id);
				} else {
					err.accumulated_errors += "Unknown ideology " + std::string(text) + " in file " + err.file_name + " line " + std::to_string(line) + "\n";
				}
			}

		}

		void finish(government_type_context&) { }
	};

	struct governments_file {
		void finish(scenario_building_context&) { }
	};

	void make_government(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);

	struct cb_list {
		void free_value(std::string_view text, error_handler& err, int32_t line, scenario_building_context& context) {
			dcon::cb_type_id new_id = context.state.world.create_cb_type();
			context.map_of_cb_types.insert_or_assign(std::string(text), pending_cb_content{ token_generator{}, new_id });
		}
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
		void organisation(association_type, float value, error_handler& err, int32_t line, trait_context& context) {
			context.outer_context.state.world.leader_trait_set_organisation(context.id, value);
		}
		void morale(association_type, float value, error_handler& err, int32_t line, trait_context& context) {
			context.outer_context.state.world.leader_trait_set_morale(context.id, value);
		}
		void attack(association_type, float value, error_handler& err, int32_t line, trait_context& context) {
			context.outer_context.state.world.leader_trait_set_attack(context.id, value);
		}
		void defence(association_type, float value, error_handler& err, int32_t line, trait_context& context) {
			context.outer_context.state.world.leader_trait_set_defence(context.id, value);
		}
		void reconnaissance(association_type, float value, error_handler& err, int32_t line, trait_context& context) {
			context.outer_context.state.world.leader_trait_set_reconnaissance(context.id, value);
		}
		void speed(association_type, float value, error_handler& err, int32_t line, trait_context& context) {
			context.outer_context.state.world.leader_trait_set_speed(context.id, value);
		}
		void experience(association_type, float value, error_handler& err, int32_t line, trait_context& context) {
			context.outer_context.state.world.leader_trait_set_experience(context.id, value);
		}
		void reliability(association_type, float value, error_handler& err, int32_t line, trait_context& context) {
			context.outer_context.state.world.leader_trait_set_reliability(context.id, value);
		}
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
		triggered_modifier_context(scenario_building_context& outer_context, uint32_t index, std::string_view name) : outer_context(outer_context), index(index), name(name) {}
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
		void free_value(int32_t value, error_handler& err, int32_t line, scenario_building_context& context) {
			if(size_t(value - 1) > context.prov_id_to_original_id_map.size()) {
				err.accumulated_errors += "Province id " + std::to_string(value) + " is too large (" + err.file_name + " line " + std::to_string(line) + ")\n";
			} else {
				context.prov_id_to_original_id_map[dcon::province_id(dcon::province_id::value_base_t(value - 1))].is_sea = true;
			}
		}
		void finish(scenario_building_context&) { }
	};

	struct default_map_file {
		sea_list sea_starts;

		void max_provinces(association_type, int32_t value, error_handler& err, int32_t line, scenario_building_context& context) {
			context.state.world.province_resize(int32_t(value - 1));
			context.original_id_to_prov_id_map.resize(value);
			context.prov_id_to_original_id_map.resize(value - 1);

			for(int32_t i = 1; i < value; ++i) {
				context.prov_id_to_original_id_map[dcon::province_id(dcon::province_id::value_base_t(i - 1))].id = i;
			}
		}

		void finish(scenario_building_context&);
	};

	void read_map_colors(char const* start, char const* end, error_handler& err, scenario_building_context& context);


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
		void free_value(int32_t value, error_handler& err, int32_t line, state_def_building_context& context) {
			if(size_t(value) >= context.outer_context.original_id_to_prov_id_map.size()) {
				err.accumulated_errors += "Province id " + std::to_string(value) + " is too large (" + err.file_name + " line " + std::to_string(line) + ")\n";
			} else {
				auto province_id = context.outer_context.original_id_to_prov_id_map[value];
				context.outer_context.state.world.force_create_abstract_state_membership(province_id, context.id);
			}
		}
		void finish(state_def_building_context&) { }
	};

	struct region_file {
		void finish(scenario_building_context&) { }
	};

	void make_state_definition(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);

	struct continent_building_context {
		scenario_building_context& outer_context;
		dcon::modifier_id id;
	};

	struct continent_provinces {
		void free_value(int32_t value, error_handler& err, int32_t line, continent_building_context& context) {
			if(size_t(value) >= context.outer_context.original_id_to_prov_id_map.size()) {
				err.accumulated_errors += "Province id " + std::to_string(value) + " is too large (" + err.file_name + " line " + std::to_string(line) + ")\n";
			} else {
				auto province_id = context.outer_context.original_id_to_prov_id_map[value];
				context.outer_context.state.world.province_set_continent(province_id, context.id);
			}
		}
		void finish(continent_building_context&) { }
	};

	struct continent_definition : public modifier_base {
		continent_provinces provinces;
		void finish(continent_building_context&) { }
	};

	struct continent_file {
		void finish(scenario_building_context&) { }
	};

	void make_continent_definition(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);

	struct climate_building_context {
		scenario_building_context& outer_context;
		dcon::modifier_id id;
	};

	struct climate_definition : public modifier_base {
		void free_value(int32_t value, error_handler& err, int32_t line, climate_building_context& context) {
			if(size_t(value) >= context.outer_context.original_id_to_prov_id_map.size()) {
				err.accumulated_errors += "Province id " + std::to_string(value) + " is too large (" + err.file_name + " line " + std::to_string(line) + ")\n";
			} else {
				auto province_id = context.outer_context.original_id_to_prov_id_map[value];
				context.outer_context.state.world.province_set_climate(province_id, context.id);
			}
		}
		void finish(climate_building_context&) { }
	};

	struct climate_file {
		void finish(scenario_building_context&) { }
	};

	void make_climate_definition(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);

	struct tech_group_context {
		scenario_building_context& outer_context;
		::culture::tech_category category = ::culture::tech_category::army;
		tech_group_context(scenario_building_context& outer_context, ::culture::tech_category category) : outer_context(outer_context), category(category) { }
	};
	struct tech_folder_list {
		void free_value(std::string_view name, error_handler& err, int32_t line, tech_group_context& context) {
			auto name_id = text::find_or_add_key(context.outer_context.state, name);
			auto cindex = context.outer_context.state.culture_definitions.tech_folders.size();
			context.outer_context.state.culture_definitions.tech_folders.push_back(::culture::folder_info{ name_id , context.category });
			context.outer_context.map_of_tech_folders.insert_or_assign(std::string(name), int32_t(cindex));
		}
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

	void register_invention(std::string_view name, token_generator& gen, error_handler& err, tech_group_context& context); //but not at the patent office

	struct commodity_set : public economy::commodity_set {
		int32_t num_added = 0;
		void any_value(std::string_view name, association_type, float value, error_handler& err, int32_t line, scenario_building_context& context) {
			auto found_commodity = context.map_of_commodity_names.find(std::string(name));
			if(found_commodity != context.map_of_commodity_names.end()) {
				if(num_added < int32_t(economy::commodity_set::set_size)) {
					commodity_amounts[num_added] = value;
					commodity_type[num_added] = found_commodity->second;
					++num_added;
				} else {
					err.accumulated_errors += "Too many items in a commodity set, in file " + err.file_name + " line " + std::to_string(line) + "\n";
				}
			} else {
				err.accumulated_errors += "Unknown commodity " + std::string(name) + " in file " + err.file_name + " line " + std::to_string(line) + "\n";
			}
		}

		void finish(scenario_building_context&) { }
	};

	struct unit_definition : public military::unit_definition {
		void unit_type_text(association_type, std::string_view value, error_handler& err, int32_t line, scenario_building_context& context) {
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
				err.accumulated_errors += std::string(value) + " is not a valid unit type (" + err.file_name + " line " + std::to_string(line) + ")\n";

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
		void ideology(association_type, std::string_view text, error_handler& err, int32_t line, party_context& context) {
			if(auto it = context.outer_context.map_of_ideologies.find(std::string(text)); it != context.outer_context.map_of_ideologies.end()) {
				context.outer_context.state.world.political_party_set_ideology(context.id, it->second.id);
			} else {
				err.accumulated_errors += std::string(text) + " is not a valid ideology (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
		void name(association_type, std::string_view text, error_handler& err, int32_t line, party_context& context) {
			auto name_id = text::find_or_add_key(context.outer_context.state, text);
			context.outer_context.state.world.political_party_set_name(context.id, name_id);
		}
		void start_date(association_type, sys::year_month_day ymd, error_handler& err, int32_t line, party_context& context) {
			auto date_tag = sys::date(ymd, context.outer_context.state.start_date);
			context.outer_context.state.world.political_party_set_start_date(context.id, date_tag);
		}
		void end_date(association_type, sys::year_month_day ymd, error_handler& err, int32_t line, party_context& context) {
			auto date_tag = sys::date(ymd, context.outer_context.state.start_date);
			context.outer_context.state.world.political_party_set_end_date(context.id, date_tag);
		}
		void any_value(std::string_view issue, association_type, std::string_view option, error_handler& err, int32_t line, party_context& context) {
			if(auto it = context.outer_context.map_of_issues.find(std::string(issue)); it != context.outer_context.map_of_issues.end()) {
				if(it->second.index() < int32_t(context.outer_context.state.culture_definitions.party_issues.size())) {
					if(auto oit = context.outer_context.map_of_options.find(std::string(option)); oit != context.outer_context.map_of_options.end()) {
						int32_t found = -1;
						for(int32_t i = 0; i < ::culture::max_issue_options; ++i) {
							if(context.outer_context.state.world.issue_get_options(it->second)[i] == oit->second.id) {
								found = i;
								break;
							}
						}
						if(found == -1) {
							err.accumulated_errors += std::string(option) + " is not an option for " + std::string(issue) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
							return;
						}
						context.outer_context.state.world.political_party_set_party_issues(context.id, it->second, uint8_t(found));
					} else {
						err.accumulated_errors += std::string(option) + " is not a valid option name (" + err.file_name + " line " + std::to_string(line) + ")\n";
					}
				} else {
					err.accumulated_errors += std::string(issue) + " is not a proper party issue (" + err.file_name + " line " + std::to_string(line) + ")\n";
				}
				//context.outer_context.state.world.political_party_set_ideology(context.id, it->second.id);
			} else {
				err.accumulated_errors += std::string(issue) + " is not a valid issue name (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
		void finish(party_context&) { }
	};
	struct unit_names_list {
		void free_value(std::string_view text, error_handler& err, int32_t line, unit_names_context& context) {
			if(text.length() <= 2)
				return;

			auto existing_count = context.outer_context.state.world.national_identity_get_unit_names_count(context.id, context.u_id);
			if(existing_count == 0) {
				auto first_id = context.outer_context.state.add_unit_name(text);
				context.outer_context.state.world.national_identity_set_unit_names_first(context.id, context.u_id, first_id);
				context.outer_context.state.world.national_identity_set_unit_names_count(context.id, context.u_id, uint8_t(1));
			} else {
				context.outer_context.state.add_unit_name(text);
				context.outer_context.state.world.national_identity_set_unit_names_count(context.id, context.u_id, uint8_t(existing_count + 1));
			}
		}
		void finish(unit_names_context&) { }
	};
	struct unit_names_collection {
		void finish(country_file_context&) { }
	};
	struct country_file {
		void color(color_from_3i cvalue, error_handler& err, int32_t line, country_file_context& context) {
			context.outer_context.state.world.national_identity_set_color(context.id, cvalue.value);
		}
		unit_names_collection unit_names;
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
			if(auto it = context.outer_context.map_of_ideologies.find(std::string(text)); it != context.outer_context.map_of_ideologies.end()) {
				id = it->second.id;
			} else {
				err.accumulated_errors += std::string(text) + " is not a valid ideology name (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
		void finish(province_file_context&) { }
	};
	struct pv_state_building {
		int32_t level = 1;
		dcon::factory_type_id id;
		void building(association_type, std::string_view text, error_handler& err, int32_t line, province_file_context& context) {
			if(auto it = context.outer_context.map_of_factory_names.find(std::string(text)); it != context.outer_context.map_of_factory_names.end()) {
				id = it->second;
			} else {
				err.accumulated_errors += std::string(text) + " is not a valid factory name (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
		void finish(province_file_context&) { }
	};

	struct province_history_file {
		void life_rating(association_type, uint32_t value, error_handler& err, int32_t line, province_file_context& context) {
			context.outer_context.state.world.province_set_life_rating(context.id, uint8_t(value));
		}
		void fort(association_type, uint32_t value, error_handler& err, int32_t line, province_file_context& context) {
			context.outer_context.state.world.province_set_fort_naval_base_level(context.id,
				province::set_fort_level(value, context.outer_context.state.world.province_get_fort_naval_base_level(context.id))
				);
		}
		void naval_base(association_type, uint32_t value, error_handler& err, int32_t line, province_file_context& context) {
			context.outer_context.state.world.province_set_fort_naval_base_level(context.id,
				province::set_naval_base_level(value, context.outer_context.state.world.province_get_fort_naval_base_level(context.id))
			);
		}
		void railroad(association_type, uint32_t value, error_handler& err, int32_t line, province_file_context& context) {
			context.outer_context.state.world.province_set_railroad_level(context.id, uint8_t(value));
		}
		void colony(association_type, uint32_t value, error_handler& err, int32_t line, province_file_context& context) {
			context.outer_context.state.world.province_set_is_colonial(context.id, value != 0);
		}
		void trade_goods(association_type, std::string_view text, error_handler& err, int32_t line, province_file_context& context) {
			if(auto it = context.outer_context.map_of_commodity_names.find(std::string(text)); it != context.outer_context.map_of_commodity_names.end()) {
				context.outer_context.state.world.province_set_rgo(context.id, it->second);
			} else {
				err.accumulated_errors += std::string(text) + " is not a valid commodity name (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
		void owner(association_type, uint32_t value, error_handler& err, int32_t line, province_file_context& context);
		void controller(association_type, uint32_t value, error_handler& err, int32_t line, province_file_context& context);
		void terrain(association_type, std::string_view text, error_handler& err, int32_t line, province_file_context& context) {
			if(auto it = context.outer_context.map_of_terrain_types.find(std::string(text)); it != context.outer_context.map_of_terrain_types.end()) {
				context.outer_context.state.world.province_set_terrain(context.id, it->second.id);
			} else {
				err.accumulated_errors += std::string(text) + " is not a valid commodity name (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
		void add_core(association_type, uint32_t value, error_handler& err, int32_t line, province_file_context& context) {
			if(auto it = context.outer_context.map_of_ident_names.find(value); it != context.outer_context.map_of_ident_names.end()) {
				context.outer_context.state.world.try_create_core(context.id, it->second);
			} else {
				err.accumulated_errors += "Invalid tag (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
		void remove_core(association_type, uint32_t value, error_handler& err, int32_t line, province_file_context& context) {
			if(auto it = context.outer_context.map_of_ident_names.find(value); it != context.outer_context.map_of_ident_names.end()) {
				auto core = context.outer_context.state.world.get_core_by_prov_tag_key(context.id, it->second);
				context.outer_context.state.world.delete_core(core);
			} else {
				err.accumulated_errors += "Invalid tag (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
		void party_loyalty(pv_party_loyalty const& value, error_handler& err, int32_t line, province_file_context& context) {
			if(value.id) {
				context.outer_context.state.world.province_set_party_loyalty(context.id, value.id, uint8_t(value.loyalty_value));
			}
		}
		void state_building(pv_state_building const& value, error_handler& err, int32_t line, province_file_context& context) {
			if(value.id) {
				auto new_fac = context.outer_context.state.world.create_factory();
				context.outer_context.state.world.factory_set_building_type(new_fac, value.id);
				context.outer_context.state.world.factory_set_level(new_fac, uint8_t(value.level));
				context.outer_context.state.world.force_create_factory_location(new_fac, context.id);
			}
		}
		void is_slave(association_type, bool value, error_handler& err, int32_t line, province_file_context& context) {
			context.outer_context.state.world.province_set_is_slave(context.id, value);
		}
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
		void culture(association_type, std::string_view value, error_handler& err, int32_t line, pop_history_province_context& context) {
			if(auto it = context.outer_context.map_of_culture_names.find(std::string(value)); it != context.outer_context.map_of_culture_names.end()) {
				cul_id = it->second;
			} else {
				err.accumulated_errors += "Invalid culture " + std::string(value)  + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
		void religion(association_type, std::string_view value, error_handler& err, int32_t line, pop_history_province_context& context) {
			if(auto it = context.outer_context.map_of_religion_names.find(std::string(value)); it != context.outer_context.map_of_religion_names.end()) {
				rel_id = it->second;
			} else {
				err.accumulated_errors += "Invalid religion " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
		void rebel_type(association_type, std::string_view value, error_handler& err, int32_t line, pop_history_province_context& context) {
			if(auto it = context.outer_context.map_of_rebeltypes.find(std::string(value)); it != context.outer_context.map_of_rebeltypes.end()) {
				reb_id = it->second.id;
			} else {
				err.accumulated_errors += "Invalid rebel type " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
		void finish(pop_history_province_context&) { }
	};

	struct pop_province_list {
		void any_group(std::string_view type, pop_history_definition const& def, error_handler& err, int32_t line, pop_history_province_context& context) {
			dcon::pop_type_id ptype;
			if(auto it = context.outer_context.map_of_poptypes.find(std::string(type)); it != context.outer_context.map_of_poptypes.end()) {
				ptype = it->second;
			} else {
				err.accumulated_errors += "Invalid pop type " + std::string(type) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
			for(auto pops_by_location : context.outer_context.state.world.province_get_pop_location(context.id)) {
				auto pop_id = pops_by_location.get_pop();
				if(pop_id.get_culture() == def.cul_id && pop_id.get_poptype() == ptype && pop_id.get_religion() == def.rel_id) {
					pop_id.get_size() += float(def.size);
					return; // done with this pop
				}
			}
			// no existing pop matched -- make a new pop
			auto new_pop = fatten(context.outer_context.state.world, context.outer_context.state.world.create_pop());
			new_pop.set_culture(def.cul_id);
			new_pop.set_religion(def.rel_id);
			new_pop.set_size(float(def.size));
			new_pop.set_poptype(ptype);
			new_pop.set_militancy(def.militancy);
			new_pop.set_rebel_group(def.reb_id);
			context.outer_context.state.world.force_create_pop_location(new_pop, context.id);
		}
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
				err.accumulated_errors += "Invalid income type " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
		void finish(poptype_context&) { }
	};


	struct poptype_file {
		void finish(poptype_context&) { }
		void sprite(association_type, int32_t value, error_handler& err, int32_t line, poptype_context& context) {
			context.outer_context.state.world.pop_type_set_sprite(context.id, uint8_t(value));
		}
		void color(color_from_3i cvalue, error_handler& err, int32_t line, poptype_context& context) {
			context.outer_context.state.world.pop_type_set_color(context.id, cvalue.value);
		}
		void is_artisan(association_type, bool value, error_handler& err, int32_t line, poptype_context& context) {
			if(value)
				context.outer_context.state.culture_definitions.artisans = context.id;
		}
		void strata(association_type, std::string_view value, error_handler& err, int32_t line, poptype_context& context) {
			if(is_fixed_token_ci(value.data(), value.data() + value.length(), "rich"))
				context.outer_context.state.world.pop_type_set_strata(context.id, uint8_t(::culture::pop_strata::rich));
			else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "middle"))
				context.outer_context.state.world.pop_type_set_strata(context.id, uint8_t(::culture::pop_strata::middle));
			else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "poor"))
				context.outer_context.state.world.pop_type_set_strata(context.id, uint8_t(::culture::pop_strata::poor));
			else {
				err.accumulated_errors += "Invalid pop strata " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
		void unemployment(association_type, bool value, error_handler& err, int32_t line, poptype_context& context) {
			context.outer_context.state.world.pop_type_set_has_unemployment(context.id, value);
		}
		void is_slave(association_type, bool value, error_handler& err, int32_t line, poptype_context& context) {
			if(value)
				context.outer_context.state.culture_definitions.slaves = context.id;
		}
		void can_be_recruited(association_type, bool value, error_handler& err, int32_t line, poptype_context& context) {
			if(value)
				context.outer_context.state.culture_definitions.soldiers = context.id;
		}
		void leadership(association_type, int32_t value, error_handler& err, int32_t line, poptype_context& context) {
			context.outer_context.state.culture_definitions.officer_leadership_points = value;
			context.outer_context.state.culture_definitions.officers = context.id;
		}
		void research_optimum(association_type, float value, error_handler& err, int32_t line, poptype_context& context) {
			context.outer_context.state.world.pop_type_set_research_optimum(context.id, value);
		}
		void administrative_efficiency(association_type, bool value, error_handler& err, int32_t line, poptype_context& context) {
			if(value)
				context.outer_context.state.culture_definitions.bureaucrat = context.id;
		}
		void tax_eff(association_type, float value, error_handler& err, int32_t line, poptype_context& context) {
			context.outer_context.state.culture_definitions.bureaucrat_tax_efficiency = value;
		}
		void can_build(association_type, bool value, error_handler& err, int32_t line, poptype_context& context) {
			if(value)
				context.outer_context.state.culture_definitions.capitalists = context.id;
		}
		void research_points(association_type, float value, error_handler& err, int32_t line, poptype_context& context) {
			context.outer_context.state.world.pop_type_set_research_points(context.id, value);
		}
		void can_reduce_consciousness(association_type, bool value, error_handler& err, int32_t line, poptype_context& context) {
			if(value)
				context.outer_context.state.culture_definitions.clergy = context.id;
		}
		void workplace_input(association_type, float value, error_handler& err, int32_t line, poptype_context& context) {
			context.outer_context.state.world.pop_type_set_workplace_input(context.id, value);
		}
		void workplace_output(association_type, float value, error_handler& err, int32_t line, poptype_context& context) {
			context.outer_context.state.world.pop_type_set_workplace_output(context.id, value);
		}
		void equivalent(association_type, std::string_view value, error_handler& err, int32_t line, poptype_context& context) {
			if(value.length() > 0 && value[0] == 'f') {
				context.outer_context.state.culture_definitions.laborers = context.id;
			} else if(value.length() > 0 && value[0] == 'l') {
				context.outer_context.state.culture_definitions.farmers = context.id;
			}
		}
		void life_needs(commodity_array const& value, error_handler& err, int32_t line, poptype_context& context) {
			context.outer_context.state.world.for_each_commodity([&](dcon::commodity_id cid) {
				if(cid.index() < value.data.ssize())
					context.outer_context.state.world.pop_type_set_life_needs(context.id, cid, value.data[cid]);
			});
		}
		void everyday_needs(commodity_array const& value, error_handler& err, int32_t line, poptype_context& context) {
			context.outer_context.state.world.for_each_commodity([&](dcon::commodity_id cid) {
				if(cid.index() < value.data.ssize())
					context.outer_context.state.world.pop_type_set_everyday_needs(context.id, cid, value.data[cid]);
			});
		}
		void luxury_needs(commodity_array const& value, error_handler& err, int32_t line, poptype_context& context) {
			context.outer_context.state.world.for_each_commodity([&](dcon::commodity_id cid) {
				if(cid.index() < value.data.ssize())
					context.outer_context.state.world.pop_type_set_luxury_needs(context.id, cid, value.data[cid]);
			});
		}
		promotion_targets promote_to;
		pop_ideologies ideologies;
		pop_issues issues;

		void life_needs_income(income const& value, error_handler& err, int32_t line, poptype_context& context) {
			context.outer_context.state.world.pop_type_set_life_needs_income_weight(context.id, value.weight);
			context.outer_context.state.world.pop_type_set_life_needs_income_type(context.id, uint8_t(value.itype));
		}
		void everyday_needs_income(income const& value, error_handler& err, int32_t line, poptype_context& context) {
			context.outer_context.state.world.pop_type_set_everyday_needs_income_weight(context.id, value.weight);
			context.outer_context.state.world.pop_type_set_everyday_needs_income_type(context.id, uint8_t(value.itype));
		}
		void luxury_needs_income(income const& value, error_handler& err, int32_t line, poptype_context& context) {
			context.outer_context.state.world.pop_type_set_luxury_needs_income_weight(context.id, value.weight);
			context.outer_context.state.world.pop_type_set_luxury_needs_income_type(context.id, uint8_t(value.itype));
		}
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
		void can_reduce_militancy(association_type, bool value, error_handler& err, int32_t line, individual_ideology_context& context) {
			if(value)
				context.outer_context.state.culture_definitions.conservative = context.id;
		}
		void uncivilized(association_type, bool value, error_handler& err, int32_t line, individual_ideology_context& context) {
			context.outer_context.state.world.ideology_set_is_civilized_only(context.id, !value);
		}
		void color(color_from_3i cvalue, error_handler& err, int32_t line, individual_ideology_context& context) {
			context.outer_context.state.world.ideology_set_color(context.id, cvalue.value);
		}
		void date(association_type, sys::year_month_day ymd, error_handler& err, int32_t line, individual_ideology_context& context) {
			auto date_tag = sys::date(ymd, context.outer_context.state.start_date);
			context.outer_context.state.world.ideology_set_activation_date(context.id, date_tag);
		}
		void add_political_reform(dcon::value_modifier_key value, error_handler& err, int32_t line, individual_ideology_context& context) {
			context.outer_context.state.world.ideology_set_add_political_reform(context.id, value);
		}
		void remove_political_reform(dcon::value_modifier_key value, error_handler& err, int32_t line, individual_ideology_context& context) {
			context.outer_context.state.world.ideology_set_remove_political_reform(context.id, value);
		}
		void add_social_reform(dcon::value_modifier_key value, error_handler& err, int32_t line, individual_ideology_context& context) {
			context.outer_context.state.world.ideology_set_add_social_reform(context.id, value);
		}
		void remove_social_reform(dcon::value_modifier_key value, error_handler& err, int32_t line, individual_ideology_context& context) {
			context.outer_context.state.world.ideology_set_remove_social_reform(context.id, value);
		}
		void add_military_reform(dcon::value_modifier_key value, error_handler& err, int32_t line, individual_ideology_context& context) {
			context.outer_context.state.world.ideology_set_add_military_reform(context.id, value);
		}
		void add_economic_reform(dcon::value_modifier_key value, error_handler& err, int32_t line, individual_ideology_context& context) {
			context.outer_context.state.world.ideology_set_add_economic_reform(context.id, value);
		}
	};

	dcon::value_modifier_key ideology_condition(token_generator& gen, error_handler& err, individual_ideology_context& context);
	dcon::trigger_key read_triggered_modifier_condition(token_generator& gen, error_handler& err, scenario_building_context& context);

	struct individual_cb_context {
		scenario_building_context& outer_context;
		dcon::cb_type_id id;
	};


	struct cb_body {
		void finish(individual_cb_context&) { }
		void is_civil_war(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
			if(value) {
				context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::is_civil_war;
				context.outer_context.state.military_definitions.standard_civil_war = context.id;
			}
		}
		void months(association_type, int32_t value, error_handler& err, int32_t line, individual_cb_context& context) {
			context.outer_context.state.world.cb_type_set_months(context.id, uint8_t(value));
		}
		void truce_months(association_type, int32_t value, error_handler& err, int32_t line, individual_cb_context& context) {
			context.outer_context.state.world.cb_type_set_truce_months(context.id, uint8_t(value));
		}
		void sprite_index(association_type, int32_t value, error_handler& err, int32_t line, individual_cb_context& context) {
			context.outer_context.state.world.cb_type_set_sprite_index(context.id, uint8_t(value));
		}
		void always(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
			if(value) {
				context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::always;
			}
		}
		void is_triggered_only(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
			if(value) {
				context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::is_triggered_only;
			}
		}
		void constructing_cb(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
			if(!value) {
				context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::is_not_constructing_cb;
			}
		}
		void great_war_obligatory(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
			if(value) {
				context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::great_war_obligatory;
				context.outer_context.state.military_definitions.standard_great_war = context.id;
			}
		}
		void all_allowed_states(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
			if(value) {
				context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::all_allowed_states;
			}
		}
		void crisis(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
			if(!value) {
				context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::not_in_crisis;
			}
		}
		void po_clear_union_sphere(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
			if(value) {
				context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_clear_union_sphere;
			}
		}
		void po_gunboat(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
			if(value) {
				context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_gunboat;
			}
		}
		void po_annex(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
			if(value) {
				context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_annex;
			}
		}
		void po_demand_state(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
			if(value) {
				context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_demand_state;
			}
		}
		void po_add_to_sphere(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
			if(value) {
				context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_add_to_sphere;
			}
		}
		void po_disarmament(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
			if(value) {
				context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_disarmament;
			}
		}
		void po_reparations(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
			if(value) {
				context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_reparations;
			}
		}
		void po_transfer_provinces(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
			if(value) {
				context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_transfer_provinces;
			}
		}
		void po_remove_prestige(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
			if(value) {
				context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_remove_prestige;
			}
		}
		void po_make_puppet(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
			if(value) {
				context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_make_puppet;
			}
		}
		void po_release_puppet(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
			if(value) {
				context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_release_puppet;
			}
		}
		void po_status_quo(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
			if(value) {
				context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_status_quo;
			}
		}
		void po_install_communist_gov_type(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
			if(value) {
				context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_install_communist_gov_type;
			}
		}
		void po_uninstall_communist_gov_type(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
			if(value) {
				context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_uninstall_communist_gov_type;
			}
		}
		void po_remove_cores(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
			if(value) {
				context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_remove_cores;
			}
		}
		void po_colony(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
			if(value) {
				context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_colony;
			}
		}
		void po_destroy_forts(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
			if(value) {
				context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_destroy_forts;
			}
		}
		void po_destroy_naval_bases(association_type, bool value, error_handler& err, int32_t line, individual_cb_context& context) {
			if(value) {
				context.outer_context.state.world.cb_type_get_type_bits(context.id) |= military::cb_flag::po_destroy_naval_bases;
			}
		}
		void war_name(association_type, std::string_view value, error_handler& err, int32_t line, individual_cb_context& context) {
			context.outer_context.state.world.cb_type_set_war_name(context.id, text::find_or_add_key(context.outer_context.state, value));
		}
		void badboy_factor(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context) {
			context.outer_context.state.world.cb_type_set_badboy_factor(context.id, value);
		}
		void prestige_factor(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context) {
			context.outer_context.state.world.cb_type_set_prestige_factor(context.id, value);
		}
		void peace_cost_factor(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context) {
			context.outer_context.state.world.cb_type_set_peace_cost_factor(context.id, value);
		}
		void penalty_factor(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context) {
			context.outer_context.state.world.cb_type_set_penalty_factor(context.id, value);
		}
		void break_truce_prestige_factor(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context) {
			context.outer_context.state.world.cb_type_set_break_truce_prestige_factor(context.id, value);
		}
		void break_truce_infamy_factor(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context) {
			context.outer_context.state.world.cb_type_set_break_truce_infamy_factor(context.id, value);
		}
		void break_truce_militancy_factor(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context) {
			context.outer_context.state.world.cb_type_set_break_truce_militancy_factor(context.id, value);
		}
		void good_relation_prestige_factor(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context) {
			context.outer_context.state.world.cb_type_set_good_relation_prestige_factor(context.id, value);
		}
		void good_relation_infamy_factor(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context) {
			context.outer_context.state.world.cb_type_set_good_relation_infamy_factor(context.id, value);
		}
		void good_relation_militancy_factor(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context) {
			context.outer_context.state.world.cb_type_set_good_relation_militancy_factor(context.id, value);
		}
		void construction_speed(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context) {
			context.outer_context.state.world.cb_type_set_construction_speed(context.id, value);
		}
		void tws_battle_factor(association_type, float value, error_handler& err, int32_t line, individual_cb_context& context) {
			context.outer_context.state.world.cb_type_set_tws_battle_factor(context.id, value);
		}
		void allowed_states(dcon::trigger_key value, error_handler& err, int32_t line, individual_cb_context& context) {
			context.outer_context.state.world.cb_type_set_allowed_states(context.id, value);
		}
		void allowed_states_in_crisis(dcon::trigger_key value, error_handler& err, int32_t line, individual_cb_context& context) {
			context.outer_context.state.world.cb_type_set_allowed_states_in_crisis(context.id, value);
		}
		void allowed_substate_regions(dcon::trigger_key value, error_handler& err, int32_t line, individual_cb_context& context) {
			context.outer_context.state.world.cb_type_set_allowed_substate_regions(context.id, value);
		}
		void allowed_countries(dcon::trigger_key value, error_handler& err, int32_t line, individual_cb_context& context) {
			context.outer_context.state.world.cb_type_set_allowed_countries(context.id, value);
		}
		void can_use(dcon::trigger_key value, error_handler& err, int32_t line, individual_cb_context& context) {
			context.outer_context.state.world.cb_type_set_can_use(context.id, value);
		}
		void on_add(dcon::effect_key value, error_handler& err, int32_t line, individual_cb_context& context) {
			context.outer_context.state.world.cb_type_set_on_add(context.id, value);
		}
		void on_po_accepted(dcon::effect_key value, error_handler& err, int32_t line, individual_cb_context& context) {
			context.outer_context.state.world.cb_type_set_on_po_accepted(context.id, value);
		}
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

	struct option_rules {
		void finish(individual_option_context&) { }
		void build_factory(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::build_factory;
		}
		void expand_factory(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::expand_factory;
		}
		void open_factory(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::open_factory;
		}
		void destroy_factory(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::destroy_factory;
		}
		void factory_priority(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::factory_priority;
		}
		void can_subsidise(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::can_subsidise;
		}
		void pop_build_factory(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::pop_build_factory;
		}
		void pop_expand_factory(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::pop_expand_factory;
		}
		void pop_open_factory(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::pop_open_factory;
		}
		void delete_factory_if_no_input(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::delete_factory_if_no_input;
		}
		void build_factory_invest(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::build_factory_invest;
		}
		void expand_factory_invest(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::expand_factory_invest;
		}
		void open_factory_invest(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::open_factory_invest;
		}
		void build_railway_invest(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::build_railway_invest;
		}
		void can_invest_in_pop_projects(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::can_invest_in_pop_projects;
		}
		void pop_build_factory_invest(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::pop_build_factory_invest;
		}
		void pop_expand_factory_invest(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::pop_expand_factory_invest;
		}
		void pop_open_factory_invest(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::pop_open_factory_invest;
		}
		void allow_foreign_investment(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::allow_foreign_investment;
		}
		void slavery_allowed(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::slavery_allowed;
		}
		void primary_culture_voting(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::primary_culture_voting;
		}
		void culture_voting(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::culture_voting;
		}
		void all_voting(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::all_voting;
		}
		void largest_share(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::largest_share;
		}
		void dhont(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::dhont;
		}
		void sainte_laque(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::sainte_laque;
		}
		void same_as_ruling_party(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::same_as_ruling_party;
		}
		void rich_only(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::rich_only;
		}
		void state_vote(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::state_vote;
		}
		void population_vote(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::population_vote;
		}
		void build_railway(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.world.issue_option_get_rules(context.id) |= issue_rule::build_railway;
		}
	};

	struct on_execute_body {
		dcon::trigger_key trigger;
		dcon::effect_key effect;
		void finish(individual_option_context&) { }
	};

	struct issue_option_body : public modifier_base {
		void technology_cost(association_type, int32_t value, error_handler& err, int32_t line, individual_option_context& context) {
			context.outer_context.state.world.issue_option_set_technology_cost(context.id, value);
		}
		void war_exhaustion_effect(association_type, float value, error_handler& err, int32_t line, individual_option_context& context) {
			context.outer_context.state.world.issue_option_set_war_exhaustion_effect(context.id, value);
		}
		void administrative_multiplier(association_type, float value, error_handler& err, int32_t line, individual_option_context& context) {
			context.outer_context.state.world.issue_option_set_administrative_multiplier(context.id, value);
		}
		void is_jingoism(association_type, bool value, error_handler& err, int32_t line, individual_option_context& context) {
			if(value)
				context.outer_context.state.culture_definitions.jingoism = context.id;
		}
		void on_execute(on_execute_body const& value, error_handler& err, int32_t line, individual_option_context& context) {
			context.outer_context.state.world.issue_option_set_on_execute_trigger(context.id, value.trigger);
			context.outer_context.state.world.issue_option_set_on_execute_effect(context.id, value.effect);
		}
		option_rules rules;
	};

	void make_opt_allow(token_generator& gen, error_handler& err, individual_option_context& context);
	dcon::trigger_key make_execute_trigger(token_generator& gen, error_handler& err, individual_option_context& context);
	dcon::effect_key make_execute_effect(token_generator& gen, error_handler& err, individual_option_context& context);
	void read_pending_option(dcon::issue_option_id id, token_generator& gen, error_handler& err, scenario_building_context& context);

	struct national_focus_context {
		scenario_building_context& outer_context;
		dcon::national_focus_id id;
	};

	struct national_focus {
		void finish(national_focus_context&) { }
		void railroads(association_type, float value, error_handler& err, int32_t line, national_focus_context& context) {
			context.outer_context.state.world.national_focus_set_railroads(context.id, value);
		}
		void icon(association_type, int32_t value, error_handler& err, int32_t line, national_focus_context& context) {
			context.outer_context.state.world.national_focus_set_icon(context.id, uint8_t(value));
		}
		void limit(dcon::trigger_key value, error_handler& err, int32_t line, national_focus_context& context) {
			context.outer_context.state.world.national_focus_set_limit(context.id, value);
		}
		void has_flashpoint(association_type, bool value, error_handler& err, int32_t line, national_focus_context& context) {
			if(value)
				context.outer_context.state.national_definitions.flashpoint_focus = context.id;
		}
		void flashpoint_tension(association_type, float value, error_handler& err, int32_t line, national_focus_context& context) {
			context.outer_context.state.national_definitions.flashpoint_amount = value;
		}
		void ideology(association_type, std::string_view value, error_handler& err, int32_t line, national_focus_context& context) {
			if(auto it = context.outer_context.map_of_ideologies.find(std::string(value)); it != context.outer_context.map_of_ideologies.end()) {
				context.outer_context.state.world.national_focus_set_ideology(context.id, it->second.id);
			} else {
				err.accumulated_errors += "Invalid ideology " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
		void loyalty_value(association_type, float value, error_handler& err, int32_t line, national_focus_context& context) {
			context.outer_context.state.world.national_focus_set_loyalty_value(context.id, value);
		}
		void immigrant_attract(association_type, float value, error_handler& err, int32_t line, national_focus_context& context) {
			context.outer_context.state.world.national_focus_set_immigrant_attract(context.id, value);
		}
		void any_value(std::string_view label, association_type, float value, error_handler& err, int32_t line, national_focus_context& context) {
			std::string str_label{label};
			if(auto it = context.outer_context.map_of_poptypes.find(str_label); it != context.outer_context.map_of_poptypes.end()) {
				context.outer_context.state.world.national_focus_set_promotion_type(context.id, it->second);
				context.outer_context.state.world.national_focus_set_promotion_amount(context.id, value);
			} else if(auto itb = context.outer_context.map_of_commodity_names.find(str_label); itb != context.outer_context.map_of_commodity_names.end()) {
				context.outer_context.state.world.national_focus_set_production_focus(context.id, itb->second, value);
			} else {
				err.accumulated_errors += "Invalid pop type / commodity " + str_label + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
	};

	struct focus_group {
		void finish(scenario_building_context&) { }
	};
	struct national_focus_file {
		focus_group any_group;
		void finish(scenario_building_context&) { }
	};

	dcon::trigger_key make_focus_limit(token_generator& gen, error_handler& err, national_focus_context& context);
	void make_focus(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);

	struct main_pop_type_file {
		void finish(scenario_building_context&) { }
		void promotion_chance(dcon::value_modifier_key value, error_handler& err, int32_t line, scenario_building_context& context) {
			context.state.culture_definitions.promotion_chance = value;
		}
		void demotion_chance(dcon::value_modifier_key value, error_handler& err, int32_t line, scenario_building_context& context) {
			context.state.culture_definitions.demotion_chance = value;
		}
		void migration_chance(dcon::value_modifier_key value, error_handler& err, int32_t line, scenario_building_context& context) {
			context.state.culture_definitions.migration_chance = value;
		}
		void colonialmigration_chance(dcon::value_modifier_key value, error_handler& err, int32_t line, scenario_building_context& context) {
			context.state.culture_definitions.colonialmigration_chance = value;
		}
		void emigration_chance(dcon::value_modifier_key value, error_handler& err, int32_t line, scenario_building_context& context) {
			context.state.culture_definitions.emigration_chance = value;
		}
		void assimilation_chance(dcon::value_modifier_key value, error_handler& err, int32_t line, scenario_building_context& context) {
			context.state.culture_definitions.assimilation_chance = value;
		}
		void conversion_chance(dcon::value_modifier_key value, error_handler& err, int32_t line, scenario_building_context& context) {
			context.state.culture_definitions.conversion_chance = value;
		}
	};

	dcon::value_modifier_key make_poptype_pop_chance(token_generator& gen, error_handler& err, scenario_building_context& context);


	struct tech_context {
		scenario_building_context& outer_context;
		dcon::technology_id id;
	};

	struct unit_modifier_body : public sys::unit_modifier {
		template<typename T>
		void finish(T&) { }
	};

	struct tech_rgo_goods_output {
		void finish(tech_context&) { }
		void any_value(std::string_view label, association_type, float value, error_handler& err, int32_t line, tech_context& context) {
			if(auto it = context.outer_context.map_of_commodity_names.find(std::string(label)); it != context.outer_context.map_of_commodity_names.end()) {
				context.outer_context.state.world.technology_get_rgo_goods_output(context.id).push_back(sys::commodity_modifier{value, it->second});
			} else {
				err.accumulated_errors += "Invalid commodity " + std::string(label) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
	};
	struct tech_fac_goods_output {
		void finish(tech_context&) { }
		void any_value(std::string_view label, association_type, float value, error_handler& err, int32_t line, tech_context& context) {
			if(auto it = context.outer_context.map_of_commodity_names.find(std::string(label)); it != context.outer_context.map_of_commodity_names.end()) {
				context.outer_context.state.world.technology_get_factory_goods_output(context.id).push_back(sys::commodity_modifier{ value, it->second });
			} else {
				err.accumulated_errors += "Invalid commodity " + std::string(label) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
	};
	struct tech_rgo_size {
		void finish(tech_context&) { }
		void any_value(std::string_view label, association_type, float value, error_handler& err, int32_t line, tech_context& context) {
			if(auto it = context.outer_context.map_of_commodity_names.find(std::string(label)); it != context.outer_context.map_of_commodity_names.end()) {
				context.outer_context.state.world.technology_get_rgo_size(context.id).push_back(sys::commodity_modifier{ value, it->second });
			} else {
				err.accumulated_errors += "Invalid commodity " + std::string(label) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
	};

	struct technology_contents : public modifier_base {
		void any_group(std::string_view label, unit_modifier_body const& value, error_handler& err, int32_t line, tech_context& context) {
			if(auto it = context.outer_context.map_of_unit_types.find(std::string(label)); it != context.outer_context.map_of_unit_types.end()) {
				sys::unit_modifier temp = value;
				temp.type = it->second;
				context.outer_context.state.world.technology_get_modified_units(context.id).push_back(temp);
			} else {
				err.accumulated_errors += "Invalid unit type " + std::string(label) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
		void ai_chance(dcon::value_modifier_key value, error_handler& err, int32_t line, tech_context& context) {
			context.outer_context.state.world.technology_set_ai_chance(context.id, value);
		}
		void year(association_type, int32_t value, error_handler& err, int32_t line, tech_context& context) {
			context.outer_context.state.world.technology_set_year(context.id, int16_t(value));
		}
		void cost(association_type, int32_t value, error_handler& err, int32_t line, tech_context& context) {
			context.outer_context.state.world.technology_set_cost(context.id, value);
		}
		void area(association_type, std::string_view value, error_handler& err, int32_t line, tech_context& context) {
			if(auto it = context.outer_context.map_of_tech_folders.find(std::string(value)); it != context.outer_context.map_of_tech_folders.end()) {
				context.outer_context.state.world.technology_set_folder_index(context.id, uint8_t(it->second));
			} else {
				err.accumulated_errors += "Invalid technology folder name " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
		void max_fort(association_type, int32_t value, error_handler& err, int32_t line, tech_context& context) {
			if(value == 1) {
				context.outer_context.state.world.technology_set_increase_fort(context.id, true);
			} else {
				err.accumulated_errors += "max_fort may only be 1 (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
		void max_railroad(association_type, int32_t value, error_handler& err, int32_t line, tech_context& context) {
			if(value == 1) {
				context.outer_context.state.world.technology_set_increase_railroad(context.id, true);
			} else {
				err.accumulated_errors += "max_railroad may only be 1 (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
		void max_naval_base(association_type, int32_t value, error_handler& err, int32_t line, tech_context& context) {
			if(value == 1) {
				context.outer_context.state.world.technology_set_increase_naval_base(context.id, true);
			} else {
				err.accumulated_errors += "max_naval_base may only be 1 (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
		void colonial_points(association_type, int32_t value, error_handler& err, int32_t line, tech_context& context) {
			context.outer_context.state.world.technology_set_colonial_points(context.id, int16_t(value));
		}
		void activate_unit(association_type, std::string_view value, error_handler& err, int32_t line, tech_context& context) {
			if(auto it = context.outer_context.map_of_unit_types.find(std::string(value)); it != context.outer_context.map_of_unit_types.end()) {
				context.outer_context.state.world.technology_set_activate_unit(context.id, it->second, true);
			} else {
				err.accumulated_errors += "Invalid unit type " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
		void activate_building(association_type, std::string_view value, error_handler& err, int32_t line, tech_context& context) {
			if(is_fixed_token_ci(value.data(), value.data() + value.length(), "fort")) {
				context.outer_context.state.world.technology_set_increase_fort(context.id, true);
			} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "railroad")) {
				context.outer_context.state.world.technology_set_increase_railroad(context.id, true);
			} else if(is_fixed_token_ci(value.data(), value.data() + value.length(), "naval_base")) {
				context.outer_context.state.world.technology_set_increase_naval_base(context.id, true);
			} else if(auto it = context.outer_context.map_of_factory_names.find(std::string(value)); it != context.outer_context.map_of_factory_names.end()) {
				context.outer_context.state.world.technology_set_activate_building(context.id, it->second, true);
			} else {
				err.accumulated_errors += "Invalid factory type " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
		tech_rgo_goods_output rgo_goods_output;
		tech_rgo_size rgo_size;
		tech_fac_goods_output factory_goods_output;
	};

	dcon::value_modifier_key make_ai_chance(token_generator& gen, error_handler& err, tech_context& context);
	void read_pending_technology(dcon::technology_id id, token_generator& gen, error_handler& err, scenario_building_context& context);

	struct invention_context {
		scenario_building_context& outer_context;
		dcon::invention_id id;
	};

	struct inv_rgo_goods_output {
		void finish(invention_context&) { }
		void any_value(std::string_view label, association_type, float value, error_handler& err, int32_t line, invention_context& context) {
			if(auto it = context.outer_context.map_of_commodity_names.find(std::string(label)); it != context.outer_context.map_of_commodity_names.end()) {
				context.outer_context.state.world.invention_get_rgo_goods_output(context.id).push_back(sys::commodity_modifier{ value, it->second });
			} else {
				err.accumulated_errors += "Invalid commodity " + std::string(label) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
	};
	struct inv_fac_goods_output {
		void finish(invention_context&) { }
		void any_value(std::string_view label, association_type, float value, error_handler& err, int32_t line, invention_context& context) {
			if(auto it = context.outer_context.map_of_commodity_names.find(std::string(label)); it != context.outer_context.map_of_commodity_names.end()) {
				context.outer_context.state.world.invention_get_factory_goods_output(context.id).push_back(sys::commodity_modifier{ value, it->second });
			} else {
				err.accumulated_errors += "Invalid commodity " + std::string(label) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
	};
	struct inv_fac_goods_throughput {
		void finish(invention_context&) { }
		void any_value(std::string_view label, association_type, float value, error_handler& err, int32_t line, invention_context& context) {
			if(auto it = context.outer_context.map_of_commodity_names.find(std::string(label)); it != context.outer_context.map_of_commodity_names.end()) {
				context.outer_context.state.world.invention_get_factory_goods_throughput(context.id).push_back(sys::commodity_modifier{ value, it->second });
			} else {
				err.accumulated_errors += "Invalid commodity " + std::string(label) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
	};
	struct inv_rebel_org_gain {
		void finish(invention_context&) { }
		dcon::rebel_type_id faction_;
		float value = 0.0f;
		void faction(association_type, std::string_view v, error_handler& err, int32_t line, invention_context& context) {
			if(is_fixed_token_ci(v.data(), v.data() + v.size(), "all")) {
				// do nothing
			} else if(auto it = context.outer_context.map_of_rebeltypes.find(std::string(v)); it != context.outer_context.map_of_rebeltypes.end()) {
				faction_ = it->second.id;
			} else {
				err.accumulated_errors += "Invalid rebel type " + std::string(v) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
	};
	struct inv_effect : public modifier_base {
		void any_group(std::string_view label, unit_modifier_body const& value, error_handler& err, int32_t line, invention_context& context) {
			if(auto it = context.outer_context.map_of_unit_types.find(std::string(label)); it != context.outer_context.map_of_unit_types.end()) {
				sys::unit_modifier temp = value;
				temp.type = it->second;
				context.outer_context.state.world.invention_get_modified_units(context.id).push_back(temp);
			} else {
				err.accumulated_errors += "Invalid unit type " + std::string(label) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
		void activate_unit(association_type, std::string_view value, error_handler& err, int32_t line, invention_context& context) {
			if(auto it = context.outer_context.map_of_unit_types.find(std::string(value)); it != context.outer_context.map_of_unit_types.end()) {
				context.outer_context.state.world.invention_set_activate_unit(context.id, it->second, true);
			} else {
				err.accumulated_errors += "Invalid unit type " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
		void activate_building(association_type, std::string_view value, error_handler& err, int32_t line, invention_context& context) {
			if(auto it = context.outer_context.map_of_factory_names.find(std::string(value)); it != context.outer_context.map_of_factory_names.end()) {
				context.outer_context.state.world.invention_set_activate_building(context.id, it->second, true);
			} else {
				err.accumulated_errors += "Invalid factory type " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
		inv_rgo_goods_output rgo_goods_output;
		inv_fac_goods_throughput factory_goods_throughput;
		inv_fac_goods_output factory_goods_output;

		void shared_prestige(association_type, float value, error_handler& err, int32_t line, invention_context& context) {
			context.outer_context.state.world.invention_set_shared_prestige(context.id, value);
		}
		void enable_crime(association_type, std::string_view value, error_handler& err, int32_t line, invention_context& context) {
			if(auto it = context.outer_context.map_of_crimes.find(std::string(value)); it != context.outer_context.map_of_crimes.end()) {
				context.outer_context.state.world.invention_set_activate_crime(context.id, it->second.id, true);
			} else {
				err.accumulated_errors += "Invalid crime " + std::string(value) + " (" + err.file_name + " line " + std::to_string(line) + ")\n";
			}
		}
		void gas_attack(association_type, bool value, error_handler& err, int32_t line, invention_context& context) {
			context.outer_context.state.world.invention_set_enable_gas_attack(context.id, value);
		}
		void gas_defence(association_type, bool value, error_handler& err, int32_t line, invention_context& context) {
			context.outer_context.state.world.invention_set_enable_gas_defence(context.id, value);
		}
		void rebel_org_gain(inv_rebel_org_gain const& value, error_handler& err, int32_t line, invention_context& context) {
			context.outer_context.state.world.invention_get_rebel_org(context.id).push_back(sys::rebel_org_modifier{value.value, value.faction_});
		}
	};

	struct invention_contents : public modifier_base {
		void limit(dcon::trigger_key value, error_handler& err, int32_t line, invention_context& context) {
			context.outer_context.state.world.invention_set_limit(context.id, value);
		}
		void chance(dcon::value_modifier_key value, error_handler& err, int32_t line, invention_context& context) {
			context.outer_context.state.world.invention_set_chance(context.id, value);
		}
		void effect(inv_effect const& value, error_handler& err, int32_t line, invention_context& context) {
			for(uint32_t i = 0; i < value.next_to_add; ++i) {
				if(next_to_add >= sys::modifier_definition_size) {
					err.accumulated_errors += "Too many modifiers attached to invention (" + err.file_name + " line " + std::to_string(line) + ")\n";
					break;
				}
				constructed_definition.offsets[next_to_add] = value.constructed_definition.offsets[i];
				constructed_definition.values[next_to_add] = value.constructed_definition.values[i];
				++next_to_add;
			}
		}
	};

	dcon::value_modifier_key make_inv_chance(token_generator& gen, error_handler& err, invention_context& context);
	dcon::trigger_key make_inv_limit(token_generator& gen, error_handler& err, invention_context& context);
	void read_pending_invention(dcon::invention_id id, token_generator& gen, error_handler& err, scenario_building_context& context);

	struct s_on_yearly_pulse {
		void finish(scenario_building_context&) { }
		void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
			int32_t value = parse_int(chance, line, err);
			if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
				context.state.national_definitions.on_yearly_pulse.push_back(nations::fixed_event{int16_t(value), it->second.id });
			} else {
				auto id = context.state.world.create_national_event();
				context.map_of_national_events.insert_or_assign(event, pending_nat_event{ id, trigger::slot_contents::nation, trigger::slot_contents::nation, trigger::slot_contents::empty });
				context.state.national_definitions.on_yearly_pulse.push_back(nations::fixed_event{ int16_t(value), id });
			}
		}
	};

	struct s_on_quarterly_pulse {
		void finish(scenario_building_context&) { }
		void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
			int32_t value = parse_int(chance, line, err);
			if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
				context.state.national_definitions.on_quarterly_pulse.push_back(nations::fixed_event{ int16_t(value), it->second.id });
			} else {
				auto id = context.state.world.create_national_event();
				context.map_of_national_events.insert_or_assign(event, pending_nat_event{ id, trigger::slot_contents::nation, trigger::slot_contents::nation, trigger::slot_contents::empty });
				context.state.national_definitions.on_quarterly_pulse.push_back(nations::fixed_event{ int16_t(value), id });
			}
		}
	};

	struct s_on_battle_won {
		void finish(scenario_building_context&) { }
		void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
			int32_t value = parse_int(chance, line, err);
			if(auto it = context.map_of_provincial_events.find(event); it != context.map_of_provincial_events.end()) {
				context.state.national_definitions.on_battle_won.push_back(nations::fixed_province_event{ int16_t(value), it->second.id });
			} else {
				auto id = context.state.world.create_provincial_event();
				context.map_of_provincial_events.insert_or_assign(event, pending_prov_event{ id, trigger::slot_contents::province, trigger::slot_contents::nation, trigger::slot_contents::nation });
				context.state.national_definitions.on_battle_won.push_back(nations::fixed_province_event{ int16_t(value), id });
			}
		}
	};

	struct s_on_battle_lost {
		void finish(scenario_building_context&) { }
		void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
			int32_t value = parse_int(chance, line, err);
			if(auto it = context.map_of_provincial_events.find(event); it != context.map_of_provincial_events.end()) {
				context.state.national_definitions.on_battle_lost.push_back(nations::fixed_province_event{ int16_t(value), it->second.id });
			} else {
				auto id = context.state.world.create_provincial_event();
				context.map_of_provincial_events.insert_or_assign(event, pending_prov_event{ id, trigger::slot_contents::province, trigger::slot_contents::nation, trigger::slot_contents::nation });
				context.state.national_definitions.on_battle_lost.push_back(nations::fixed_province_event{ int16_t(value), id });
			}
		}
	};

	struct s_on_surrender {
		void finish(scenario_building_context&) { }
		void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
			int32_t value = parse_int(chance, line, err);
			if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
				context.state.national_definitions.on_surrender.push_back(nations::fixed_event{ int16_t(value), it->second.id });
			} else {
				auto id = context.state.world.create_national_event();
				context.map_of_national_events.insert_or_assign(event, pending_nat_event{ id, trigger::slot_contents::nation, trigger::slot_contents::nation, trigger::slot_contents::nation });
				context.state.national_definitions.on_surrender.push_back(nations::fixed_event{ int16_t(value), id });
			}
		}
	};

	struct s_on_new_great_nation {
		void finish(scenario_building_context&) { }
		void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
			int32_t value = parse_int(chance, line, err);
			if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
				context.state.national_definitions.on_new_great_nation.push_back(nations::fixed_event{ int16_t(value), it->second.id });
			} else {
				auto id = context.state.world.create_national_event();
				context.map_of_national_events.insert_or_assign(event, pending_nat_event{ id, trigger::slot_contents::nation, trigger::slot_contents::nation, trigger::slot_contents::empty });
				context.state.national_definitions.on_new_great_nation.push_back(nations::fixed_event{ int16_t(value), id });
			}
		}
	};

	struct s_on_lost_great_nation {
		void finish(scenario_building_context&) { }
		void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
			int32_t value = parse_int(chance, line, err);
			if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
				context.state.national_definitions.on_lost_great_nation.push_back(nations::fixed_event{ int16_t(value), it->second.id });
			} else {
				auto id = context.state.world.create_national_event();
				context.map_of_national_events.insert_or_assign(event, pending_nat_event{ id, trigger::slot_contents::nation, trigger::slot_contents::nation, trigger::slot_contents::empty });
				context.state.national_definitions.on_lost_great_nation.push_back(nations::fixed_event{ int16_t(value), id });
			}
		}
	};

	struct s_on_election_tick {
		void finish(scenario_building_context&) { }
		void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
			int32_t value = parse_int(chance, line, err);
			if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
				context.state.national_definitions.on_election_tick.push_back(nations::fixed_event{ int16_t(value), it->second.id });
			} else {
				auto id = context.state.world.create_national_event();
				context.map_of_national_events.insert_or_assign(event, pending_nat_event{ id, trigger::slot_contents::nation, trigger::slot_contents::nation, trigger::slot_contents::empty });
				context.state.national_definitions.on_election_tick.push_back(nations::fixed_event{ int16_t(value), id });
			}
		}
	};

	struct s_on_colony_to_state {
		void finish(scenario_building_context&) { }
		void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
			int32_t value = parse_int(chance, line, err);
			if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
				context.state.national_definitions.on_colony_to_state.push_back(nations::fixed_event{ int16_t(value), it->second.id });
			} else {
				auto id = context.state.world.create_national_event();
				context.map_of_national_events.insert_or_assign(event, pending_nat_event{ id, trigger::slot_contents::state, trigger::slot_contents::nation, trigger::slot_contents::empty });
				context.state.national_definitions.on_colony_to_state.push_back(nations::fixed_event{ int16_t(value), id });
			}
		}
	};

	struct s_on_state_conquest {
		void finish(scenario_building_context&) { }
		void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
			int32_t value = parse_int(chance, line, err);
			if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
				context.state.national_definitions.on_state_conquest.push_back(nations::fixed_event{ int16_t(value), it->second.id });
			} else {
				auto id = context.state.world.create_national_event();
				context.map_of_national_events.insert_or_assign(event, pending_nat_event{ id, trigger::slot_contents::state, trigger::slot_contents::nation, trigger::slot_contents::empty });
				context.state.national_definitions.on_state_conquest.push_back(nations::fixed_event{ int16_t(value), id });
			}
		}
	};

	struct s_on_colony_to_state_free_slaves {
		void finish(scenario_building_context&) { }
		void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
			int32_t value = parse_int(chance, line, err);
			if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
				context.state.national_definitions.on_colony_to_state_free_slaves.push_back(nations::fixed_event{ int16_t(value), it->second.id });
			} else {
				auto id = context.state.world.create_national_event();
				context.map_of_national_events.insert_or_assign(event, pending_nat_event{ id, trigger::slot_contents::state, trigger::slot_contents::nation, trigger::slot_contents::empty });
				context.state.national_definitions.on_colony_to_state_free_slaves.push_back(nations::fixed_event{ int16_t(value), id });
			}
		}
	};

	struct s_on_debtor_default {
		void finish(scenario_building_context&) { }
		void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
			int32_t value = parse_int(chance, line, err);
			if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
				context.state.national_definitions.on_debtor_default.push_back(nations::fixed_event{ int16_t(value), it->second.id });
			} else {
				auto id = context.state.world.create_national_event();
				context.map_of_national_events.insert_or_assign(event, pending_nat_event{ id, trigger::slot_contents::nation, trigger::slot_contents::nation, trigger::slot_contents::nation });
				context.state.national_definitions.on_debtor_default.push_back(nations::fixed_event{ int16_t(value), id });
			}
		}
	};

	struct s_on_debtor_default_small {
		void finish(scenario_building_context&) { }
		void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
			int32_t value = parse_int(chance, line, err);
			if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
				context.state.national_definitions.on_debtor_default_small.push_back(nations::fixed_event{ int16_t(value), it->second.id });
			} else {
				auto id = context.state.world.create_national_event();
				context.map_of_national_events.insert_or_assign(event, pending_nat_event{ id, trigger::slot_contents::nation, trigger::slot_contents::nation, trigger::slot_contents::nation });
				context.state.national_definitions.on_debtor_default_small.push_back(nations::fixed_event{ int16_t(value), id });
			}
		}
	};

	struct s_on_debtor_default_second {
		void finish(scenario_building_context&) { }
		void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
			int32_t value = parse_int(chance, line, err);
			if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
				context.state.national_definitions.on_debtor_default_second.push_back(nations::fixed_event{ int16_t(value), it->second.id });
			} else {
				auto id = context.state.world.create_national_event();
				context.map_of_national_events.insert_or_assign(event, pending_nat_event{ id, trigger::slot_contents::nation, trigger::slot_contents::nation, trigger::slot_contents::nation });
				context.state.national_definitions.on_debtor_default_second.push_back(nations::fixed_event{ int16_t(value), id });
			}
		}
	};

	struct s_on_civilize {
		void finish(scenario_building_context&) { }
		void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
			int32_t value = parse_int(chance, line, err);
			if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
				context.state.national_definitions.on_civilize.push_back(nations::fixed_event{ int16_t(value), it->second.id });
			} else {
				auto id = context.state.world.create_national_event();
				context.map_of_national_events.insert_or_assign(event, pending_nat_event{ id, trigger::slot_contents::nation, trigger::slot_contents::nation, trigger::slot_contents::empty });
				context.state.national_definitions.on_civilize.push_back(nations::fixed_event{ int16_t(value), id });
			}
		}
	};

	struct s_on_crisis_declare_interest {
		void finish(scenario_building_context&) { }
		void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
			int32_t value = parse_int(chance, line, err);
			if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
				context.state.national_definitions.on_crisis_declare_interest.push_back(nations::fixed_event{ int16_t(value), it->second.id });
			} else {
				auto id = context.state.world.create_national_event();
				context.map_of_national_events.insert_or_assign(event, pending_nat_event{ id, trigger::slot_contents::nation, trigger::slot_contents::nation, trigger::slot_contents::empty });
				context.state.national_definitions.on_crisis_declare_interest.push_back(nations::fixed_event{ int16_t(value), id });
			}
		}
	};

	struct s_on_my_factories_nationalized {
		void finish(scenario_building_context&) { }
		void any_value(std::string_view chance, association_type, int32_t event, error_handler& err, int32_t line, scenario_building_context& context) {
			int32_t value = parse_int(chance, line, err);
			if(auto it = context.map_of_national_events.find(event); it != context.map_of_national_events.end()) {
				context.state.national_definitions.on_my_factories_nationalized.push_back(nations::fixed_event{ int16_t(value), it->second.id });
			} else {
				auto id = context.state.world.create_national_event();
				context.map_of_national_events.insert_or_assign(event, pending_nat_event{ id, trigger::slot_contents::nation, trigger::slot_contents::nation, trigger::slot_contents::nation });
				context.state.national_definitions.on_my_factories_nationalized.push_back(nations::fixed_event{ int16_t(value), id });
			}
		}
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
}

#include "trigger_parsing.hpp"
#include "effect_parsing.hpp"
#include "parser_defs_generated.hpp"

