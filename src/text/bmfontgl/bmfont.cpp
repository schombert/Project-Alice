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
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
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

bool BMFont::ParseFont(simple_fs::file& f) {
	auto content = simple_fs::view_contents(f);

	std::stringstream Stream(std::string(content.data, content.file_size));
	std::string Line;
	std::string Read, Key, Value;
	std::size_t i;

	CharDescriptor C;

	while(!Stream.eof()) {
		std::stringstream LineStream;
		std::getline(Stream, Line);
		LineStream << Line;

		// read the line's type
		LineStream >> Read;
		if(Read == "common") {
			// this holds common data
			while(!LineStream.eof()) {
				std::stringstream Converter;
				LineStream >> Read;
				i = Read.find('=');
				Key = Read.substr(0, i);
				Value = Read.substr(i + 1);

				// assign the correct value
				Converter << Value;
				if(Key == "lineHeight") {
					Converter >> LineHeight;
				} else if(Key == "base") {
					Converter >> Base;
				} else if(Key == "scaleW") {
					Converter >> Width;
				} else if(Key == "scaleH") {
					Converter >> Height;
				} else if(Key == "pages") {
					int16_t Pages = 0;
					Converter >> Pages;
				} else if(Key == "outline") {
					int16_t Outline = 0;
					Converter >> Outline;
				}
			}
		}

		else if(Read == "char") {
			// This is data for each specific character.
			int CharID = 0;

			while(!LineStream.eof()) {
				std::stringstream Converter;
				LineStream >> Read;
				i = Read.find('=');
				Key = Read.substr(0, i);
				Value = Read.substr(i + 1);

				// Assign the correct value
				Converter << Value;
				if(Key == "id") {
					Converter >> CharID;
				} else if(Key == "x") {
					Converter >> C.x;
				} else if(Key == "y") {
					Converter >> C.y;
				} else if(Key == "width") {
					Converter >> C.Width;
				} else if(Key == "height") {
					Converter >> C.Height;
				} else if(Key == "xoffset") {
					Converter >> C.XOffset;
				} else if(Key == "yoffset") {
					Converter >> C.YOffset;
				} else if(Key == "xadvance") {
					Converter >> C.XAdvance;
				} else if(Key == "page") {
					Converter >> C.Page;
				}
			}

			Chars[uint8_t(CharID)] = C;

		} else if(Read == "kernings") {
			while(!LineStream.eof()) {
				std::stringstream Converter;
				LineStream >> Read;
				i = Read.find('=');
				Key = Read.substr(0, i);
				Value = Read.substr(i + 1);

				// assign the correct value
				Converter << Value;
				if(Key == "count") {
					int16_t KernCount = 0;
					Converter >> KernCount;
				}
			}
		} else if(Read == "kerning") {
			int32_t first = 0;
			int32_t second = 0;
			int32_t amount = 0;

			while(!LineStream.eof()) {
				std::stringstream Converter;
				LineStream >> Read;
				i = Read.find('=');
				Key = Read.substr(0, i);
				Value = Read.substr(i + 1);

				// assign the correct value
				Converter << Value;
				if(Key == "first") {
					Converter >> first;
				}

				else if(Key == "second") {
					Converter >> second;
				}

				else if(Key == "amount") {
					Converter >> amount;
				}
			}
			uint16_t index = uint16_t(uint8_t(first) << 8) | uint16_t((uint8_t(second)));
			Kern.insert_or_assign(index, amount);
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

float BMFont::GetStringWidth(char const* string, uint32_t count) const {
	float total = 0;

	for(uint32_t i = 0; i < count; ++i) {
		auto c = uint8_t(string[i]);
		if(c == 0x01 || c == 0x02 || c == 0x40)
			c = 0x4D;
		total += Chars[c].XAdvance;
		if(i != 0) {
			total += GetKerningPair(string[i - 1], c);
		}
		if(c == 0x40) // Handle @TAG
			i += 3;
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
