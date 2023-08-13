/*
BASIC bm_font example implementation with Kerning, for C++ and OpenGL 2.0

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
--------------------------------------------------------------------------------
These editors can be used to generate BMFonts:
 • http://www.angelcode.com/products/bmfont/ (free, windows)
 • http://glyphdesigner.71squared.com/ (commercial, mac os x)
 • http://www.n4te.com/hiero/hiero.jnlp (free, java, multiplatform)
 • http://slick.cokeandcode.com/demos/hiero.jnlp (free, java, multiplatform)

Some code below based on code snippets from this gamedev posting:

http://www.gamedev.net/topic/330742-quick-tutorial-variable-width-bitmap-fonts/

Although I'm giving this away, I'd appreciate an email with fixes or better code!

aaedev@gmail.com 2012
*/

#include <stdio.h>
#include <stdlib.h>
#include <glew.h>
#include <string>
#include <stdarg.h>
#include "bmfont.hpp"
#include "texture.hpp"
#include "fonts.hpp"
#include "system_state.hpp"

#ifdef _WIN64
#pragma warning(disable : 4996)
#endif

#include "parsers.hpp"

namespace parsers {
struct bmfont_file_context {
	scenario_building_context& outer_context;
	text::bm_font& font;
	uint8_t char_id = 0;
	int32_t first = 0;
	int32_t second = 0;

	bmfont_file_context(scenario_building_context& outer_context, text::bm_font& font) : outer_context(outer_context), font(font) { }
};
struct bmfont_file {
	void x(association_type, int32_t value, error_handler& err, int32_t line, bmfont_file_context& context) {
		context.font.chars[context.char_id].x = value;
	}
	void y(association_type, int32_t value, error_handler& err, int32_t line, bmfont_file_context& context) {
		context.font.chars[context.char_id].y = value;
	}
	void xadvance(association_type, int32_t value, error_handler& err, int32_t line, bmfont_file_context& context) {
		context.font.chars[context.char_id].x_advance = value;
	}
	void xoffset(association_type, int32_t value, error_handler& err, int32_t line, bmfont_file_context& context) {
		context.font.chars[context.char_id].x_offset = value;
	}
	void yoffset(association_type, int32_t value, error_handler& err, int32_t line, bmfont_file_context& context) {
		context.font.chars[context.char_id].y_offset = value;
	}
	void page(association_type, int32_t value, error_handler& err, int32_t line, bmfont_file_context& context) {
		context.font.chars[context.char_id].page = value;
	}
	void width(association_type, int32_t value, error_handler& err, int32_t line, bmfont_file_context& context) {
		context.font.chars[context.char_id].width = value;
	}
	void height(association_type, int32_t value, error_handler& err, int32_t line, bmfont_file_context& context) {
		context.font.chars[context.char_id].height = value;
	}
	void first(association_type, int32_t value, error_handler& err, int32_t line, bmfont_file_context& context) {
		context.first = value;
	}
	void second(association_type, int32_t value, error_handler& err, int32_t line, bmfont_file_context& context) {
		context.second = value;
	}
	void amount(association_type, int32_t value, error_handler& err, int32_t line, bmfont_file_context& context) {
		uint16_t index = (uint16_t(context.first) << 8) | uint16_t(context.second);
		context.font.kernings.insert_or_assign(index, value);
	}
	void id(association_type, int32_t value, error_handler& err, int32_t line, bmfont_file_context& context) {
		context.char_id = uint8_t(value);
	}
	void lineheight(association_type, int32_t value, error_handler& err, int32_t line, bmfont_file_context& context) {
		context.font.line_height = value;	
	}
	void finish(bmfont_file_context& context) {
		assert(context.font.line_height >= 0);
	}
};
}
#include "bmfont_defs_generated.hpp"

namespace text {

bool bm_font::parse_font(sys::state& state, simple_fs::file& f) {
	auto content = simple_fs::view_contents(f);
	parsers::error_handler err("");
	parsers::scenario_building_context context(state);
	err.file_name = simple_fs::get_full_name(f);
	parsers::token_generator gen(content.data, content.data + content.file_size);
	parsers::bmfont_file_context bmfont_file_context(context, *this);
	parsers::parse_bmfont_file(gen, err, bmfont_file_context);
	return true;
}

int bm_font::get_kerning_pair(char first, char second) const {
	uint16_t index = (uint16_t(first) << 8) | uint16_t(second);
	if(auto it = kernings.find(index); it != kernings.end())
		return it->second;
	else
		return 0;
}

float bm_font::get_string_width(sys::state& state, char const* string, uint32_t count) const {
	float total = 0;

	for(uint32_t i = 0; i < count; ++i) {
		auto c = uint8_t(string[i]);
		if(c == 0x01 || c == 0x02 || c == 0x40)
			c = 0x4D;
		total += chars[c].x_advance;
		if(i != 0) {
			total += get_kerning_pair(string[i - 1], c);
		}
		if(uint8_t(string[i]) == 0x40) { // Handle @TAG
			char tag[3] = { 0, 0, 0 };
			tag[0] = (i + 1 < count) ? char(string[i + 1]) : 0;
			tag[1] = (i + 2 < count) ? char(string[i + 2]) : 0;
			tag[2] = (i + 3 < count) ? char(string[i + 3]) : 0;
			if(ogl::display_tag_is_valid(state, tag))
				i += 3;
		}
	}
	return total;
}

bm_font::~bm_font() {
	if(ftexid)
		glDeleteTextures(1, &ftexid);
}

bm_font const& get_bm_font(sys::state& state, uint16_t font_handle) {
	if(auto it = state.font_collection.bitmap_fonts.find(font_handle); it != state.font_collection.bitmap_fonts.end()) {
		return it->second;
	} else {
		auto fit = state.font_collection.font_names.find(font_handle);
		assert(fit != state.font_collection.font_names.end());
		auto fname = [&]() {
			auto sv = state.to_string_view(fit->second);
			if(sv == "Main_14")
				return std::string("Arial14");
			if(sv == "ToolTip_Font")
				return std::string("Arial14");
			if(sv == "FPS_Font")
				return std::string("Arial14");
			return std::string(sv);
		}();

		auto root = get_root(state.common_fs);
		auto gfx_dir = open_directory(root, NATIVE("gfx"));
		auto font_dir = open_directory(gfx_dir, NATIVE("fonts"));

		auto font_def = open_file(font_dir, simple_fs::win1250_to_native(fname + ".fnt"));
		auto font_image = open_file(font_dir, simple_fs::win1250_to_native(fname + ".tga"));
		assert(bool(font_def) && bool(font_image));

		auto result = state.font_collection.bitmap_fonts.insert_or_assign(font_handle, bm_font(state, *font_def, *font_image));
		return result.first->second;
	}
}

} // namespace text
