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
}

#include "parser_defs_generated.hpp"

