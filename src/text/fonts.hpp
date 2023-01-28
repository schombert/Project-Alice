#pragma once

#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"
#include "unordered_dense.h"

namespace text {

uint16_t name_into_font_id(std::string_view text);
int32_t size_from_font_id(uint16_t id);
bool is_black_from_font_id(uint16_t id);
uint32_t font_index_from_font_id(uint16_t id);

struct glyph_sub_offset {
	float x = 0.0f;
	float y = 0.0f;
};

class font {
public:
	FT_Face font_face;
	float internal_line_height = 0.0f;
	float internal_ascender = 0.0f;
	float internal_descender = 0.0f;
	float internal_top_adj = 0.0f;

	bool loaded = false;

	float glyph_advances[256] = { 0.0f };
	uint32_t textures[4] = { 0, 0, 0, 0 };
	bool glyph_loaded[256] = { false };
	glyph_sub_offset glyph_positions[256] = {};

	std::unique_ptr<FT_Byte[]> file_data;

	~font();

	void make_glyph(char ch_in);
	float line_height(int32_t size) const;
	float ascender(int32_t size) const;
	float descender(int32_t size) const;
	float top_adjustment(int32_t size) const;
	float kerning(char codepoint_first, char codepoint_second) const;
	float text_extent(const char* codepoints, uint32_t count, int32_t size) const;
};

class font_manager {
public:
	font_manager();
	~font_manager();

	font fonts[2];

	FT_Library ft_library;

	void load_font(font& fnt, char* file_data, uint32_t file_size);
};

void load_standard_fonts(sys::state& state);

}
