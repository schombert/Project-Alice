#pragma once

#include "freetype/freetype.h"
#include "freetype/ftglyph.h"
#include "unordered_dense.h"
#include "hb.h"
#include "bmfont.hpp"
#include <span>

namespace sys {
struct state;
}

namespace text {

inline constexpr uint32_t max_texture_layers = 256;
inline constexpr int magnification_factor = 4;
inline constexpr int dr_size = 64 * magnification_factor;

enum class font_selection {
	body_font,
	header_font,
	map_font
};

uint16_t name_into_font_id(sys::state& state, std::string_view text);
int32_t size_from_font_id(uint16_t id);
bool is_black_from_font_id(uint16_t id);
font_selection font_index_from_font_id(sys::state& state, uint16_t id);

struct glyph_sub_offset {
	float x = 0.0f;
	float y = 0.0f;
	float x_advance = 0.0f;
	uint16_t texture_slot = 0;
};

class font_manager;

enum class font_feature {
	none, small_caps
};

class font;

inline bool requires_surrogate_pair(uint32_t codepoint) {
	return codepoint >= 0x10000;
}

struct surrogate_pair {
	uint16_t high = 0; // aka leading
	uint16_t low = 0; // aka trailing
};

inline surrogate_pair make_surrogate_pair(uint32_t val) noexcept {
	uint32_t v = val - 0x10000;
	uint32_t h = ((v >> 10) & 0x03FF) | 0xD800;
	uint32_t l = (v & 0x03FF) | 0xDC00;
	return surrogate_pair{ uint16_t(h), uint16_t(l) };
}

struct stored_glyph {
	uint32_t codepoint = 0;
	uint32_t cluster = 0;
	hb_position_t  x_advance = 0;
	hb_position_t  y_advance = 0;
	hb_position_t  x_offset = 0;
	hb_position_t  y_offset = 0;

	stored_glyph() noexcept = default;
	stored_glyph(hb_glyph_info_t const& gi, hb_glyph_position_t const& gp) {
		codepoint = gi.codepoint;
		cluster = gi.cluster;
		x_advance = gp.x_advance;
		y_advance = gp.y_advance;
		x_offset = gp.x_offset;
		y_offset = gp.y_offset;
	}
};

struct stored_glyphs {
	std::vector<stored_glyph> glyph_info;

	struct no_bidi { };

	stored_glyphs() = default;
	stored_glyphs(stored_glyphs const& other) noexcept = default;
	stored_glyphs(stored_glyphs&& other) noexcept = default;
	stored_glyphs(sys::state& state, font_selection type, std::string const& s);
	stored_glyphs(std::string const& s, font& f);
	stored_glyphs(stored_glyphs& other, uint32_t offset, uint32_t count);
	stored_glyphs(sys::state& state, font_selection type, std::span<uint16_t> s);
	stored_glyphs(sys::state& state, font_selection type, std::span<uint16_t> s, no_bidi);

	void set_text(sys::state& state, font_selection type, std::string const& s);
	void clear() {
		glyph_info.clear();
	}
};

class font {
private:
	font(font const&) = delete;
	font& operator=(font const&) = delete;
public:
	font() = default;

	std::string file_name;
	FT_Face font_face = nullptr;
	hb_font_t* hb_font_face = nullptr;
	hb_buffer_t* hb_buf = nullptr;

	float internal_line_height = 0.0f;
	float internal_ascender = 0.0f;
	float internal_descender = 0.0f;
	float internal_top_adj = 0.0f;
	ankerl::unordered_dense::map<char32_t, glyph_sub_offset> glyph_positions;
	std::vector<uint32_t> textures;
	std::array<FT_ULong, 256> win1252_codepoints;

	uint16_t first_free_slot = 0;
	std::unique_ptr<FT_Byte[]> file_data;
	bool only_raw_codepoints = false;

	~font();
	bool can_display(char32_t ch_in) const;
	void make_glyph(char32_t ch_in);
	float base_glyph_width(char32_t ch_in);
	float line_height(int32_t size) const;
	float ascender(int32_t size) const;
	float descender(int32_t size) const;
	float top_adjustment(int32_t size) const;
	float text_extent(sys::state& state, stored_glyphs const& txt, uint32_t starting_offset, uint32_t count, int32_t size);
	void remake_cache(sys::state& state, font_selection type, stored_glyphs& txt, std::string const& source);
	void remake_cache(stored_glyphs& txt, std::string const& source);
	void remake_cache(sys::state& state, font_selection type, stored_glyphs& txt, std::span<uint16_t> source);
	void remake_bidiless_cache(sys::state& state, font_selection type, stored_glyphs& txt, std::span<uint16_t> source);

	friend class font_manager;

	font(font&& o) noexcept : file_name(std::move(o.file_name)), textures(std::move(o.textures)), glyph_positions(std::move(o.glyph_positions)), file_data(std::move(o.file_data)), first_free_slot(o.first_free_slot), only_raw_codepoints(o.only_raw_codepoints) {
		font_face = o.font_face;
		o.font_face = nullptr;
		hb_font_face = o.hb_font_face;
		o.hb_font_face = nullptr;
		hb_buf = o.hb_buf;
		o.hb_buf = nullptr;
		internal_line_height = o.internal_line_height;
		internal_ascender = o.internal_ascender;
		internal_descender = o.internal_descender;
		internal_top_adj = o.internal_top_adj;
	}
	font& operator=(font&& o) noexcept {
		file_name = std::move(o.file_name);
		file_data = std::move(o.file_data);
		glyph_positions = std::move(o.glyph_positions);
		textures = std::move(o.textures);
		font_face = o.font_face;
		o.font_face = nullptr;
		hb_font_face = o.hb_font_face;
		o.hb_font_face = nullptr;
		hb_buf = o.hb_buf;
		o.hb_buf = nullptr;
		internal_line_height = o.internal_line_height;
		internal_ascender = o.internal_ascender;
		internal_descender = o.internal_descender;
		internal_top_adj = o.internal_top_adj;
		first_free_slot = o.first_free_slot;
		only_raw_codepoints = o.only_raw_codepoints;
	}
};

class font_manager {
public:
	font_manager();
	~font_manager();

	ankerl::unordered_dense::map<uint16_t, dcon::text_key> font_names;
	ankerl::unordered_dense::map<uint16_t, bm_font> bitmap_fonts;
	FT_Library ft_library;
private:
	std::vector<font> font_array;
	dcon::locale_id current_locale;
public:
	std::vector<uint8_t> compiled_ubrk_rules;
	bool map_font_is_black = false;

	dcon::locale_id get_current_locale() const {
		return current_locale;
	}
	void change_locale(sys::state& state, dcon::locale_id l);
	font& get_font(sys::state& state, font_selection s = font_selection::body_font);
	void load_font(font& fnt, char const* file_data, uint32_t file_size);
	float line_height(sys::state& state, uint16_t font_id);
	float text_extent(sys::state& state, stored_glyphs const& txt, uint32_t starting_offset, uint32_t count, uint16_t font_id);
	void set_classic_fonts(bool v);
};

std::string_view classic_unligate_utf8(text::font& font, char32_t c);

} // namespace text
