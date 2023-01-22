#pragma once

#include <stdint.h>
#include <optional>
#include <string_view>
#include <string>

#include "parsers.hpp"
#include "unordered_dense.h"

namespace parsers {

	//
	// structures and functions for parsing .gfx files
	//

	struct building_gfx_context {
		ui::defintions* ui_defs = nullptr;
		ankerl::unordered_dense::map<std::string, dcon::gfx_object_id>* map_of_names = nullptr;
	};

	struct gfx_xy_pair {
		int32_t x = 0;
		int32_t y = 0;

		void finish(building_gfx_context const& context) { }
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

		void clicksound(parsers::association_type, std::string_view, parsers::error_handler& err, int32_t line, building_gfx_context const& context) {
			clicksound_set = true;
		}

		void finish(building_gfx_context const& context) {
		}
	};

	struct gfx_object_outer {
		void spritetype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context const& context);
		void corneredtilespritetype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context const& context);
		void maskedshieldtype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context const& context);
		void textspritetype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context const& context);
		void tilespritetype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context const& context);
		void progressbartype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context const& context);
		void barcharttype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context const& context);
		void piecharttype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context const& context);
		void linecharttype(gfx_object const& obj, parsers::error_handler& err, int32_t line, building_gfx_context const& context);

		void finish(building_gfx_context const& context) {
		}
	};

	void gfx_add_obj(parsers::token_generator& gen, parsers::error_handler& err, building_gfx_context const& context);

	struct gfx_files {
		void finish(building_gfx_context const& context) {
		}
	};

}

#include "parser_defs_generated.hpp"

