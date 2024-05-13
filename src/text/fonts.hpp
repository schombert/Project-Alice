#pragma once

#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"
#include "unordered_dense.h"
#include "bmfont.hpp"

namespace sys {
struct state;
}

namespace text {

uint16_t name_into_font_id(sys::state& state, std::string_view text);
int32_t size_from_font_id(uint16_t id);
bool is_black_from_font_id(uint16_t id);
uint32_t font_index_from_font_id(uint16_t id);

struct glyph_sub_offset {
	float x = 0.0f;
	float y = 0.0f;
};

class font_manager;


enum class font_feature {
	none, small_caps
};

class font {
private:
	font(font const&) = delete;
	font(font&&) = default;
	font& operator=(font const&) = delete;
	font& operator=(font&&) noexcept = default;
	font() = default;

public:
	FT_Face font_face;
	ankerl::unordered_dense::map<uint16_t, float> kernings;
	std::vector<uint16_t> substitution_indices;
	std::vector<uint8_t const*> type_2_kerning_tables;
	uint8_t const* gs = nullptr;

	float internal_line_height = 0.0f;
	float internal_ascender = 0.0f;
	float internal_descender = 0.0f;
	float internal_top_adj = 0.0f;

	font_feature features = font_feature::none;
	bool loaded = false;

	float glyph_advances[256] = {0.0f};
	uint32_t textures[4] = {0, 0, 0, 0};
	bool glyph_loaded[256] = {false};
	glyph_sub_offset glyph_positions[256] = {};

	std::unique_ptr<FT_Byte[]> file_data;

	~font();

	void make_glyph(char ch_in);
	float line_height(int32_t size) const;
	float ascender(int32_t size) const;
	float descender(int32_t size) const;
	float top_adjustment(int32_t size) const;
	float kerning(char codepoint_first, char codepoint_second);
	float text_extent(sys::state& state, char const* codepoints, uint32_t count, int32_t size);

	friend class font_manager;
};

class font_manager {
public:
	font_manager();
	~font_manager();

	font fonts[3];
	ankerl::unordered_dense::map<uint16_t, dcon::text_key> font_names;
	ankerl::unordered_dense::map<uint16_t, bm_font> bitmap_fonts;
	FT_Library ft_library;

	void load_font(font& fnt, char const* file_data, uint32_t file_size, font_feature f);
	void load_all_glyphs();

	float line_height(sys::state& state, uint16_t font_id) const;
	float text_extent(sys::state& state, char const* codepoints, uint32_t count, uint16_t font_id);
};

void load_standard_fonts(sys::state& state);
void load_bmfonts(sys::state& state);
} // namespace text
