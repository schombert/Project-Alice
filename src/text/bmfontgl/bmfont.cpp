/*
BASIC BMFont example implementation with Kerning, for C++ and OpenGL 2.0

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
#include "bmfont.h"
#include <stdarg.h>

#include "texture.hpp"
#include "fonts.hpp"
#include "system_state.hpp"

#ifdef _WIN64
#pragma warning(disable : 4996)
#endif

// Todo: Add buffer overflow checking.

namespace text {

const char *skip_whitespace(const char *start, const char *end) {
	assert(start < end);
	for(; start != end && (*start == ' ' || *start == '\t' || *start == '\n' || *start == '\r'); start++)
		;
	return start;
}

const char *read_int(const char *start, const char *end, int32_t& v) {
	assert(start < end);
	std::from_chars(start, end, v);
	for(; start != end && (*start >= '0' && *start <= '9'); start++)
		;
	return start;
}

bool BMFont::ParseFont(simple_fs::file& f) {
	auto content = simple_fs::view_contents(f);

	auto const* seq = content.data;
	auto const* end_seq = content.data + content.file_size;

	int32_t rvalue = 0;
	uint32_t char_id = 0;
	int32_t kern_first = 0;
	int32_t kern_second = 0;
	int32_t kern_amount = 0;
	while(seq != end_seq) {
		assert(seq < end_seq);
		seq = skip_whitespace(seq, end_seq);
		if(parsers::has_fixed_suffix_ci(seq, end_seq, "lineheight")) {
			seq += 10;
			if(seq != end_seq && *seq == '=') {
				++seq;
				seq = skip_whitespace(seq, end_seq);
				seq = read_int(seq, end_seq, rvalue);
				LineHeight = int16_t(rvalue);
			}
		} else if(parsers::has_fixed_suffix_ci(seq, end_seq, "kernings")) {
			seq += 8;
			while(seq != end_seq && *seq != '\n')
				seq++;
		} else if(parsers::has_fixed_suffix_ci(seq, end_seq, "xadvance")) {
			seq += 8;
			seq = skip_whitespace(seq, end_seq);
			if(seq != end_seq && *seq == '=') {
				++seq;
				seq = skip_whitespace(seq, end_seq);
				seq = read_int(seq, end_seq, rvalue);
				Chars[char_id].XAdvance = short(rvalue);
			}
		} else if(parsers::has_fixed_suffix_ci(seq, end_seq, "xoffset")) {
			seq += 7;
			seq = skip_whitespace(seq, end_seq);
			if(seq != end_seq && *seq == '=') {
				++seq;
				seq = skip_whitespace(seq, end_seq);
				seq = read_int(seq, end_seq, rvalue);
				Chars[char_id].XOffset = short(rvalue);
			}
		} else if(parsers::has_fixed_suffix_ci(seq, end_seq, "yoffset")) {
			seq += 7;
			seq = skip_whitespace(seq, end_seq);
			if(seq != end_seq && *seq == '=') {
				++seq;
				seq = skip_whitespace(seq, end_seq);
				seq = read_int(seq, end_seq, rvalue);
				Chars[char_id].YOffset = short(rvalue);
			}
		} else if(parsers::has_fixed_suffix_ci(seq, end_seq, "kerning")) {
			seq += 7;
			seq = skip_whitespace(seq, end_seq);
		} else if(parsers::has_fixed_suffix_ci(seq, end_seq, "common")) {
			seq += 6;
			seq = skip_whitespace(seq, end_seq);
		} else if(parsers::has_fixed_suffix_ci(seq, end_seq, "height")) {
			seq += 6;
			seq = skip_whitespace(seq, end_seq);
			if(seq != end_seq && *seq == '=') {
				++seq;
				seq = skip_whitespace(seq, end_seq);
				seq = read_int(seq, end_seq, rvalue);
				Chars[char_id].Height = short(rvalue);
			}
		} else if(parsers::has_fixed_suffix_ci(seq, end_seq, "amount")) {
			seq += 6;
			seq = skip_whitespace(seq, end_seq);
			if(seq != end_seq && *seq == '=') {
				++seq;
				seq = skip_whitespace(seq, end_seq);
				seq = read_int(seq, end_seq, rvalue);
				kern_amount = rvalue;

				uint16_t index = uint16_t(uint8_t(kern_first) << 8) | uint16_t((uint8_t(kern_second)));
				Kern.insert_or_assign(index, kern_amount);
			}
		} else if(parsers::has_fixed_suffix_ci(seq, end_seq, "second")) {
			seq += 6;
			seq = skip_whitespace(seq, end_seq);
			if(seq != end_seq && *seq == '=') {
				++seq;
				seq = skip_whitespace(seq, end_seq);
				seq = read_int(seq, end_seq, rvalue);
				kern_second = rvalue;
			}
		} else if(parsers::has_fixed_suffix_ci(seq, end_seq, "scalew")) {
			seq += 6;
			seq = read_int(seq, end_seq, rvalue);
			ScaleW = int16_t(rvalue);
		} else if(parsers::has_fixed_suffix_ci(seq, end_seq, "scaleh")) {
			seq += 6;
			seq = read_int(seq, end_seq, rvalue);
			ScaleH = int16_t(rvalue);
		} else if(parsers::has_fixed_suffix_ci(seq, end_seq, "pages")) {
			seq += 5;
			seq = read_int(seq, end_seq, rvalue);
			Pages = int16_t(rvalue);
		} else if(parsers::has_fixed_suffix_ci(seq, end_seq, "width")) {
			seq += 5;
			seq = skip_whitespace(seq, end_seq);
			if(seq != end_seq && *seq == '=') {
				++seq;
				seq = skip_whitespace(seq, end_seq);
				seq = read_int(seq, end_seq, rvalue);
				Chars[char_id].Width = short(rvalue);
			}
		} else if(parsers::has_fixed_suffix_ci(seq, end_seq, "first")) {
			seq += 5;
			seq = skip_whitespace(seq, end_seq);
			if(seq != end_seq && *seq == '=') {
				++seq;
				seq = skip_whitespace(seq, end_seq);
				seq = read_int(seq, end_seq, rvalue);
				kern_first = short(rvalue);
			}
		} else if(parsers::has_fixed_suffix_ci(seq, end_seq, "base")) {
			seq += 4;
			seq = read_int(seq, end_seq, rvalue);
			Base = int16_t(rvalue);
		} else if(parsers::has_fixed_suffix_ci(seq, end_seq, "info")) {
			seq += 4;
			while(seq != end_seq && *seq != '\n')
				seq++;
		} else if(parsers::has_fixed_suffix_ci(seq, end_seq, "page")) {
			seq += 4;
			seq = skip_whitespace(seq, end_seq);
			if(seq != end_seq && *seq == '=') {
				++seq;
				seq = skip_whitespace(seq, end_seq);
				seq = read_int(seq, end_seq, rvalue);
				Chars[char_id].Page = short(rvalue);
			}
		} else if(parsers::has_fixed_suffix_ci(seq, end_seq, "char")) {
			seq += 4;
			seq = skip_whitespace(seq, end_seq);
		} else if(parsers::has_fixed_suffix_ci(seq, end_seq, "id")) {
			seq += 2;
			seq = skip_whitespace(seq, end_seq);
			if(seq != end_seq && *seq == '=') {
				++seq;
				seq = skip_whitespace(seq, end_seq);
				seq = read_int(seq, end_seq, rvalue);
				char_id = uint8_t(rvalue);
			}
		} else if(parsers::has_fixed_suffix_ci(seq, end_seq, "x")) {
			seq += 1;
			seq = skip_whitespace(seq, end_seq);
			if(seq != end_seq && *seq == '=') {
				++seq;
				seq = skip_whitespace(seq, end_seq);
				seq = read_int(seq, end_seq, rvalue);
				Chars[char_id].x = short(rvalue);
			}
		} else if(parsers::has_fixed_suffix_ci(seq, end_seq, "y")) {
			seq += 1;
			seq = skip_whitespace(seq, end_seq);
			if(seq != end_seq && *seq == '=') {
				++seq;
				seq = skip_whitespace(seq, end_seq);
				seq = read_int(seq, end_seq, rvalue);
				Chars[char_id].y = short(rvalue);
			}
		} else if(seq != end_seq) {
			seq++;
		}
	}
	return true;
}

int BMFont::GetKerningPair(char first, char second) const {
	uint16_t index = uint16_t(uint8_t(first) << 8) | uint16_t((uint8_t(second)));
	if(auto it = this->Kern.find(index); it != Kern.end())
		return it->second;
	else
		return 0;
}

float BMFont::GetStringWidth(sys::state& state, char const* string, uint32_t count) const {
	float total = 0;

	for(uint32_t i = 0; i < count; ++i) {
		auto c = uint8_t(string[i]);
		if(c == 0x01 || c == 0x02 || c == 0x40)
			c = 0x4D;
		total += Chars[c].XAdvance;
		if(i != 0) {
			total += GetKerningPair(string[i - 1], c);
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

BMFont::~BMFont() {
	if(ftexid)
		glDeleteTextures(1, &ftexid);
}

BMFont const& get_bm_font(sys::state& state, uint16_t font_handle) {
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

		auto result = state.font_collection.bitmap_fonts.insert_or_assign(font_handle, BMFont(*font_def, *font_image));
		return result.first->second;
	}
}

} // namespace text
