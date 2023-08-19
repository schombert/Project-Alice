/*
bm_font example implementation with Kerning, for C++ and OpenGL 2.0

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
*/

#pragma once

#include <vector>
#include "simple_fs.hpp"
#include "texture.hpp"
#include "unordered_dense.h"

namespace sys {
struct state;
}

namespace text {

struct char_descriptor {
	int32_t x = 0;
	int32_t y = 0;
	int32_t width = 0;
	int32_t height = 0;
	int32_t x_offset = 0;
	int32_t y_offset = 0;
	int32_t x_advance = 0;
	int32_t page = 0;
};

class bm_font {
public:
	bm_font(sys::state& state, simple_fs::file& font_metrics, simple_fs::file& font_image) {
		auto font_result = ogl::make_font_texture(font_image);
		ftexid = font_result.handle;
		parse_font(state, font_metrics);
		assert(ftexid != 0);
		width = int16_t(font_result.size);
	};
	bm_font(bm_font&& src) noexcept {
		ftexid = src.ftexid;
		chars = src.chars;
		kernings = std::move(src.kernings);
		width = src.width;
		height = src.height;
		base = src.base;
		line_height = src.line_height;
		src.ftexid = 0;
	}
	bm_font& operator=(bm_font&& src) noexcept {
		ftexid = src.ftexid;
		chars = src.chars;
		kernings = std::move(src.kernings);
		width = src.width;
		height = src.height;
		base = src.base;
		line_height = src.line_height;
		src.ftexid = 0;
		return *this;
	}
	~bm_font();

	std::array<char_descriptor, 256> chars;
	ankerl::unordered_dense::map<uint16_t, int32_t> kernings;
	int32_t line_height = 0;
	int32_t base = 0;
	int32_t width = 0;
	int32_t height = 0;
	int32_t pages = 0;
	int32_t scale_w = 0;
	int32_t scale_h = 0;
	GLuint ftexid = 0;

	float get_height() const { return float(line_height); }
	float get_string_width(sys::state& state, char const*, uint32_t) const;
	bool parse_font(sys::state& state, simple_fs::file& f);
	int get_kerning_pair(char, char) const;
};

bm_font const& get_bm_font(sys::state& state, uint16_t font_handle);

} // namespace text
