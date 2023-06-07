/*
BMFont example implementation with Kerning, for C++ and OpenGL 2.0

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

class CharDescriptor {

public:
	short x;
	short y;
	short Width;
	short Height;
	short XOffset;
	short YOffset;
	short XAdvance;
	short Page;

	CharDescriptor() : x(0), y(0), Width(0), Height(0), XOffset(0), YOffset(0), XAdvance(0), Page(0) { }
};

class BMFont {

public:
	GLuint ftexid = 0;

	float GetHeight() const { return LineHeight; }
	float GetStringWidth(char const*, uint32_t) const;

	BMFont(simple_fs::file& font_metrics, simple_fs::file& font_image) {
		auto font_result = ogl::make_font_texture(font_image);
		ftexid = font_result.handle;
		ParseFont(font_metrics);
		assert(ftexid != 0);
		Width = int16_t(font_result.size);
	};
	BMFont(BMFont&& src) noexcept {
		ftexid = src.ftexid;
		Chars = src.Chars;
		Kern = std::move(src.Kern);
		Width = src.Width;
		Height = src.Height;
		Base = src.Base;
		LineHeight = src.LineHeight;
		src.ftexid = 0;
	}
	BMFont& operator=(BMFont&& src) noexcept {
		ftexid = src.ftexid;
		Chars = src.Chars;
		Kern = std::move(src.Kern);
		Width = src.Width;
		Height = src.Height;
		Base = src.Base;
		LineHeight = src.LineHeight;
		src.ftexid = 0;

		return *this;
	}
	~BMFont();

	std::array<CharDescriptor, 256> Chars;
	ankerl::unordered_dense::map<uint16_t, int32_t> Kern;

	int16_t LineHeight = 0;
	int16_t Base = 0;
	int16_t Width = 0;
	int16_t Height = 0;

	bool ParseFont(simple_fs::file& f);
	int GetKerningPair(char, char) const;
};

BMFont const& get_bm_font(sys::state& state, uint16_t font_handle);

} // namespace text
