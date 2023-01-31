#pragma once

#include <stdint.h>
#include <optional>
#include <string_view>
#include <string>

#include "parsers.hpp"
#include "unordered_dense.h"
#include "gui_graphics.hpp"

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

	struct scenario_building_context {
		sys::state& state;

		ankerl::unordered_dense::map<uint32_t, dcon::national_identity_id> map_of_ident_names;
		tagged_vector<std::string, dcon::national_identity_id> file_names_for_idents;

		ankerl::unordered_dense::map<std::string, dcon::religion_id> map_of_religion_names;
		ankerl::unordered_dense::map<std::string, dcon::culture_id> map_of_culture_names;
		ankerl::unordered_dense::map<std::string, dcon::culture_group_id> map_of_culture_group_names;
		ankerl::unordered_dense::map<std::string, dcon::building_id> map_of_building_names;

		scenario_building_context(sys::state& state) : state(state) { }
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

	struct colonial_points_context {
		dcon::building_id id;
		scenario_building_context& outer_context;
	};
	struct colonial_points_list {
		template<typename T>
		void free_value(int32_t v, error_handler& err, int32_t line, T& context) {
			context.outer_context.state.world.building_get_colonial_points(context.id).push_back(v);
		}
		void finish(colonial_points_context& context) { }
	};
	struct building_context {
		dcon::building_id id;
		scenario_building_context& outer_context;
	};
	struct goods_cost_list {
		template<typename T>
		void any_value(std::string_view name, association_type, int32_t v, error_handler& err, int32_t line, T& context) {
			// TODO add goods in here
		}
		void finish(building_context& context) { }
	};
	struct building {
		void type(association_type, std::string_view name, error_handler& err, int32_t line, building_context& context) {
			if(name == "factory")
				context.outer_context.state.world.building_set_type(context.id, uint8_t(sys::building_type::factory));
			else if(name == "fort")
				context.outer_context.state.world.building_set_type(context.id, uint8_t(sys::building_type::fort));
			else if(name == "naval_base")
				context.outer_context.state.world.building_set_type(context.id, uint8_t(sys::building_type::naval_base));
			else if(name == "infrastructure")
				context.outer_context.state.world.building_set_type(context.id, uint8_t(sys::building_type::infrastructure));
			else {
				err.accumulated_errors += "Unknown building type " + std::string(name) + " in file " + err.file_name + " line " + std::to_string(line) + "\n";
			}
		}
		void on_completion(association_type, bool v, error_handler& err, int32_t line, building_context& context) {
			context.outer_context.state.world.building_set_on_completion(context.id, v);
		}
		void completion_size(association_type, int32_t v, error_handler& err, int32_t line, building_context& context) {
			context.outer_context.state.world.building_set_completion_size(context.id, v);
		}
		void max_level(association_type, int32_t v, error_handler& err, int32_t line, building_context& context) {
			context.outer_context.state.world.building_set_max_level(context.id, v);
		}
		void cost(association_type, int32_t v, error_handler& err, int32_t line, building_context& context) {
			context.outer_context.state.world.building_set_max_level(context.id, v);
		}
		void goods_cost(goods_cost_list v, error_handler& err, int32_t line, building_context& context) {
			// TODO context.outer_context.state.world.building_set_goods_cost(context.id, v);
		}
		void time(association_type, int32_t v, error_handler& err, int32_t line, building_context& context) {
			context.outer_context.state.world.building_set_time(context.id, v);
		}
		void visibility(association_type, bool v, error_handler& err, int32_t line, building_context& context) {
			context.outer_context.state.world.building_set_visibility(context.id, v);
		}
		void naval_capacity(association_type, int32_t v, error_handler& err, int32_t line, building_context& context) {
			context.outer_context.state.world.building_set_naval_capacity(context.id, v);
		}
		void capital(association_type, bool v, error_handler& err, int32_t line, building_context& context) {
			context.outer_context.state.world.building_set_capital(context.id, v);
		}
		void port(association_type, bool v, error_handler& err, int32_t line, building_context& context) {
			context.outer_context.state.world.building_set_port(context.id, v);
		}
		void onmap(association_type, bool v, error_handler& err, int32_t line, building_context& context) {
			context.outer_context.state.world.building_set_onmap(context.id, v);
		}
		void production_type(association_type, std::string_view v, error_handler& err, int32_t line, building_context& context) {
			// ignore...
		}
		void pop_build_factory(association_type, bool v, error_handler& err, int32_t line, building_context& context) {
			context.outer_context.state.world.building_set_pop_build_factory(context.id, v);
		}
		void advanced_factory(association_type, bool v, error_handler& err, int32_t line, building_context& context) {
			context.outer_context.state.world.building_set_advanced_factory(context.id, v);
		}
		void strategic_factory(association_type, bool v, error_handler& err, int32_t line, building_context& context) {
			context.outer_context.state.world.building_set_strategic_factory(context.id, v);
		}
		void province(association_type, bool v, error_handler& err, int32_t line, building_context& context) {
			context.outer_context.state.world.building_set_province(context.id, v);
		}
		void fort_level(association_type, int32_t v, error_handler& err, int32_t line, building_context& context) {
			context.outer_context.state.world.building_set_fort_level(context.id, v);
		}
		void local_ship_build(association_type, float v, error_handler& err, int32_t line, building_context& context) {
			context.outer_context.state.world.building_set_local_ship_build(context.id, v);
		}
		void movement_cost(association_type, float v, error_handler& err, int32_t line, building_context& context) {
			context.outer_context.state.world.building_set_movement_cost(context.id, v);
		}
		void infrastructure(association_type, float v, error_handler& err, int32_t line, building_context& context) {
			context.outer_context.state.world.building_set_infrastructure(context.id, v);
		}
		void colonial_range(association_type, int32_t v, error_handler& err, int32_t line, building_context& context) {
			context.outer_context.state.world.building_set_colonial_range(context.id, v);
		}
		void one_per_state(association_type, bool v, error_handler& err, int32_t line, building_context& context) {
			context.outer_context.state.world.building_set_one_per_state(context.id, v);
		}
		void spawn_railway_track(association_type, bool v, error_handler& err, int32_t line, building_context& context) {
			context.outer_context.state.world.building_set_spawn_railway_track(context.id, v);
		}
		void default_enabled(association_type, bool v, error_handler& err, int32_t line, building_context& context) {
			context.outer_context.state.world.building_set_default_enabled(context.id, v);
		}
		void sail(association_type, bool v, error_handler& err, int32_t line, building_context& context) {
			context.outer_context.state.world.building_set_sail(context.id, v);
		}
		void steam(association_type, bool v, error_handler& err, int32_t line, building_context& context) {
			context.outer_context.state.world.building_set_steam(context.id, v);
		}
		void finish(building_context& context) { }
	};
	struct building_file {
		void finish(scenario_building_context& context) { }
	};
}

#include "parser_defs_generated.hpp"

