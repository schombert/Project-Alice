#include <cmath>
#include <bit>

#include "hb.h"
#include "hb-ft.h"

#include "fonts.hpp"
#include "parsers.hpp"
#include "simple_fs.hpp"
#include "system_state.hpp"
#ifdef _WIN32
#include <icu.h>
#else
#include <unicode/ubrk.h>
#include <unicode/utypes.h>
#include <unicode/ubidi.h>
#endif

namespace text {

constexpr uint16_t pack_font_handle(uint32_t font_index, bool black, uint32_t size) {
	return uint16_t(uint32_t((font_index - 1) << 7) | uint32_t(black ? (1 << 6) : 0) | uint32_t(size & 0x3F));
}

bool is_black_font(std::string_view txt) {
	if(parsers::has_fixed_suffix_ci(txt.data(), txt.data() + txt.length(), "_bl") ||
			parsers::has_fixed_suffix_ci(txt.data(), txt.data() + txt.length(), "black") ||
			parsers::has_fixed_suffix_ci(txt.data(), txt.data() + txt.length(), "black_bold")) {
		return true;
	} else {
		return false;
	}
}

uint32_t font_size(std::string_view txt) {
	char const* first_int = txt.data();
	char const* end = txt.data() + txt.size();
	while(first_int != end && !isdigit(*first_int))
		++first_int;
	char const* last_int = first_int;
	while(last_int != end && isdigit(*last_int))
		++last_int;

	if(first_int == last_int) {
		if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "fps_font"))
			return uint32_t(14);
		else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "tooltip_font"))
			return uint32_t(16);
		else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "frangoth_bold"))
			return uint32_t(18);
		else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "impact_small"))
			return uint32_t(24);
		else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "old_english"))
			return uint32_t(50);
		else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "timefont"))
			return uint32_t(24);
		else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "vic_title"))
			return uint32_t(42);
		else
			return uint32_t(14);
	}

	uint32_t rvalue = 0;
	std::from_chars(first_int, last_int, rvalue);
	return rvalue;
}

uint32_t font_index(std::string_view txt) {
	if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "arial"))
		return 1;
	else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "fps"))
		return 1;
	else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "main"))
		return 2;
	else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "tooltip"))
		return 1;
	else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "frangoth"))
		return 2;
	else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "garamond"))
		return 2;
	else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "impact"))
		return 2;
	else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "old"))
		return 2;
	else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "timefont"))
		return 1;
	else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "vic"))
		return 2;
	else
		return 1;
}

uint16_t name_into_font_id(sys::state& state, std::string_view txt) {
	auto base_id = pack_font_handle(font_index(txt), is_black_font(txt), font_size(txt));
	std::string txt_copy = [&]() {
		if(parsers::has_fixed_suffix_ci(txt.data(), txt.data() + txt.length(), "_black")) {
			return std::string(txt.substr(0, txt.length() - 6));
		}
		if(parsers::has_fixed_suffix_ci(txt.data(), txt.data() + txt.length(), "_black_bold")) {
			return std::string(txt.substr(0, txt.length() - 11)) + "_bold";
		}
		return std::string(txt);
	}();
	uint16_t individuator = 0;
	auto it = state.font_collection.font_names.find(uint16_t(base_id | (individuator << 8)));
	while(it != state.font_collection.font_names.end()) {
		if(state.to_string_view(it->second) == txt_copy) {
			return uint16_t(base_id | (individuator << 8));
		}
		++individuator;
		it = state.font_collection.font_names.find(uint16_t(base_id | (individuator << 8)));
	}
	auto new_key = state.add_key_utf8(txt_copy);
	auto new_handle = uint16_t(base_id | (individuator << 8));
	state.font_collection.font_names.insert_or_assign(new_handle, new_key);
	return new_handle;
}

int32_t size_from_font_id(uint16_t id) {
	auto index = uint32_t(((id >> 7) & 0x01) + 1);
	if(index == 2)
		return (int32_t(id & 0x3F) * 3) / 4;
	else
		return (int32_t(id & 0x3F) * 5) / 6;
}

bool is_black_from_font_id(uint16_t id) {
	return ((id >> 6) & 0x01) != 0;
}
font_selection font_index_from_font_id(sys::state& state, uint16_t id) {
	uint32_t offset = 0;
	if(((id >> 7) & 0x01) == 0)
		return font_selection::body_font;
	else
		return font_selection::header_font;
}

float font_manager::text_extent(sys::state& state, stored_glyphs const& txt, uint32_t starting_offset, uint32_t count, uint16_t font_id) {
	auto& font = get_font(state, text::font_index_from_font_id(state, font_id));
	auto size = text::size_from_font_id(font_id);
	if(state.user_settings.use_classic_fonts) {
		std::string codepoints = "";
		for(uint32_t i = starting_offset; i < starting_offset + count; i++) {
			codepoints.push_back(char(txt.glyph_info[i].codepoint));
		}
		return text::get_bm_font(state, font_id).get_string_width(state, codepoints.c_str(), uint32_t(codepoints.size()));
	}
	return float(font.text_extent(state, txt, starting_offset, count, size));
}

float font_manager::line_height(sys::state& state, uint16_t font_id) {
	if(state.user_settings.use_classic_fonts) {
		return text::get_bm_font(state, font_id).get_height();
	}
	return float(get_font(state, text::font_index_from_font_id(state, font_id)).line_height(text::size_from_font_id(font_id)));
}

font_manager::font_manager() {
	FT_Init_FreeType(&ft_library);
}
font_manager::~font_manager() {
	FT_Done_FreeType(ft_library);
}

font::~font() {
	if(hb_buf)
		hb_buffer_destroy(hb_buf);
	// if(loaded)
	//	FT_Done_Face(font_face);
}

int32_t transform_offset_b(int32_t x, int32_t y, int32_t btmap_x_off, int32_t btmap_y_off, uint32_t width, uint32_t height,
		uint32_t pitch) {
	int bmp_x = x - btmap_x_off;
	int bmp_y = y - btmap_y_off;

	if((bmp_x < 0) || (bmp_x >= (int32_t)width) || (bmp_y < 0) || (bmp_y >= (int32_t)height))
		return -1;
	else
		return bmp_x + bmp_y * (int32_t)pitch;
}


constexpr float rt_2 = 1.41421356237309504f;

void init_in_map(bool in_map[dr_size * dr_size], uint8_t const* bmp_data, int32_t btmap_x_off, int32_t btmap_y_off, uint32_t width, uint32_t height, uint32_t pitch) {
	for(int32_t j = 0; j < dr_size; ++j) {
		for(int32_t i = 0; i < dr_size; ++i) {
			auto const boff = transform_offset_b(i, j, btmap_x_off, btmap_y_off, width, height, pitch);
			in_map[i + dr_size * j] = (boff != -1) ? (bmp_data[boff] > 127) : false;
		}
	}
}

//
// based on The "dead reckoning" signed distance transform
// Grevera, George J. (2004) Computer Vision and Image Understanding 95 pages 317–333
//

void dead_reckoning(float distance_map[dr_size * dr_size], bool const in_map[dr_size * dr_size]) {
	int16_t yborder[dr_size * dr_size] = {0};
	int16_t xborder[dr_size * dr_size] = {0};

	for(uint32_t i = 0; i < dr_size * dr_size; ++i) {
		distance_map[i] = std::numeric_limits<float>::infinity();
	}
	for(int32_t j = 1; j < dr_size - 1; ++j) {
		for(int32_t i = 1; i < dr_size - 1; ++i) {
			if(in_map[i - 1 + dr_size * j] != in_map[i + dr_size * j] || in_map[i + 1 + dr_size * j] != in_map[i + dr_size * j] ||
					in_map[i + dr_size * (j + 1)] != in_map[i + dr_size * j] || in_map[i + dr_size * (j - 1)] != in_map[i + dr_size * j]) {
				distance_map[i + dr_size * j] = 0.0f;
				yborder[i + dr_size * j] = static_cast<int16_t>(j);
				xborder[i + dr_size * j] = static_cast<int16_t>(i);
			}
		}
	}
	for(int32_t j = 1; j < dr_size - 1; ++j) {
		for(int32_t i = 1; i < dr_size - 1; ++i) {
			if(distance_map[(i - 1) + dr_size * (j - 1)] + rt_2 < distance_map[(i) + dr_size * (j)]) {
				yborder[i + dr_size * j] = yborder[(i - 1) + dr_size * (j - 1)];
				xborder[i + dr_size * j] = xborder[(i - 1) + dr_size * (j - 1)];
				distance_map[(i) + dr_size * (j)] = (float)std::sqrt((i - xborder[i + dr_size * j]) * (i - xborder[i + dr_size * j]) +
																														 (j - yborder[i + dr_size * j]) * (j - yborder[i + dr_size * j]));
			}
			if(distance_map[(i) + dr_size * (j - 1)] + 1.0f < distance_map[(i) + dr_size * (j)]) {
				yborder[i + dr_size * j] = yborder[(i) + dr_size * (j - 1)];
				xborder[i + dr_size * j] = xborder[(i) + dr_size * (j - 1)];
				distance_map[(i) + dr_size * (j)] = (float)std::sqrt((i - xborder[i + dr_size * j]) * (i - xborder[i + dr_size * j]) +
																														 (j - yborder[i + dr_size * j]) * (j - yborder[i + dr_size * j]));
			}
			if(distance_map[(i + 1) + dr_size * (j - 1)] + rt_2 < distance_map[(i) + dr_size * (j)]) {
				yborder[i + dr_size * j] = yborder[(i + 1) + dr_size * (j - 1)];
				xborder[i + dr_size * j] = xborder[(i + 1) + dr_size * (j - 1)];
				distance_map[(i) + dr_size * (j)] = (float)std::sqrt((i - xborder[i + dr_size * j]) * (i - xborder[i + dr_size * j]) +
																														 (j - yborder[i + dr_size * j]) * (j - yborder[i + dr_size * j]));
			}
			if(distance_map[(i - 1) + dr_size * (j)] + 1.0f < distance_map[(i) + dr_size * (j)]) {
				yborder[i + dr_size * j] = yborder[(i - 1) + dr_size * (j)];
				xborder[i + dr_size * j] = xborder[(i - 1) + dr_size * (j)];
				distance_map[(i) + dr_size * (j)] = (float)std::sqrt((i - xborder[i + dr_size * j]) * (i - xborder[i + dr_size * j]) +
																														 (j - yborder[i + dr_size * j]) * (j - yborder[i + dr_size * j]));
			}
		}
	}

	for(int32_t j = dr_size - 2; j > 0; --j) {
		for(int32_t i = dr_size - 2; i > 0; --i) {
			if(distance_map[(i + 1) + dr_size * (j)] + 1.0f < distance_map[(i) + dr_size * (j)]) {
				yborder[i + dr_size * j] = yborder[(i + 1) + dr_size * (j)];
				xborder[i + dr_size * j] = xborder[(i + 1) + dr_size * (j)];
				distance_map[(i) + dr_size * (j)] = (float)std::sqrt((i - xborder[i + dr_size * j]) * (i - xborder[i + dr_size * j]) +
																														 (j - yborder[i + dr_size * j]) * (j - yborder[i + dr_size * j]));
			}
			if(distance_map[(i - 1) + dr_size * (j + 1)] + rt_2 < distance_map[(i) + dr_size * (j)]) {
				yborder[i + dr_size * j] = yborder[(i - 1) + dr_size * (j + 1)];
				xborder[i + dr_size * j] = xborder[(i - 1) + dr_size * (j + 1)];
				distance_map[(i) + dr_size * (j)] = (float)std::sqrt((i - xborder[i + dr_size * j]) * (i - xborder[i + dr_size * j]) +
																														 (j - yborder[i + dr_size * j]) * (j - yborder[i + dr_size * j]));
			}
			if(distance_map[(i) + dr_size * (j + 1)] + 1.0f < distance_map[(i) + dr_size * (j)]) {
				yborder[i + dr_size * j] = yborder[(i) + dr_size * (j + 1)];
				xborder[i + dr_size * j] = xborder[(i) + dr_size * (j + 1)];
				distance_map[(i) + dr_size * (j)] = (float)std::sqrt((i - xborder[i + dr_size * j]) * (i - xborder[i + dr_size * j]) +
																														 (j - yborder[i + dr_size * j]) * (j - yborder[i + dr_size * j]));
			}
			if(distance_map[(i + 1) + dr_size * (j + 1)] + rt_2 < distance_map[(i) + dr_size * (j)]) {
				yborder[i + dr_size * j] = yborder[(i + 1) + dr_size * (j + 1)];
				xborder[i + dr_size * j] = xborder[(i + 1) + dr_size * (j + 1)];
				distance_map[(i) + dr_size * (j)] = (float)std::sqrt((i - xborder[i + dr_size * j]) * (i - xborder[i + dr_size * j]) +
																														 (j - yborder[i + dr_size * j]) * (j - yborder[i + dr_size * j]));
			}
		}
	}

	for(uint32_t i = 0; i < dr_size * dr_size; ++i) {
		if(in_map[i])
			distance_map[i] *= -1.0f;
	}
}

void font_manager::change_locale(sys::state& state, dcon::locale_id l) {
	current_locale = l;

	uint32_t end_language = 0;
	auto locale_name = state.world.locale_get_locale_name(l);
	std::string_view localename_sv((char const*)locale_name.begin(), locale_name.size());
	while(end_language < locale_name.size()) {
		if(localename_sv[end_language] == '-')
			break;
		++end_language;
	}

	std::string lang_str{ localename_sv .substr(0, end_language) };
	
	state.world.locale_set_resolved_language(l, hb_language_from_string(localename_sv.data(), int(end_language)));

	{
		auto f = state.world.locale_get_body_font(l);
		std::string fname((char const*)f.begin(), (char const*)f.end());
		font* resolved = nullptr;
		uint16_t count = 0;

		for(auto& fnt : font_array) {
			if(fnt.file_name == fname) {
				resolved = &fnt;
				break;
			}
			++count;
		}

		if(!resolved) {
			auto r = simple_fs::get_root(state.common_fs);
			auto assets = simple_fs::open_directory(r, NATIVE("assets"));
			auto fonts = simple_fs::open_directory(assets, NATIVE("fonts"));
			auto ff = simple_fs::open_file(fonts, simple_fs::utf8_to_native(fname));
			if(!ff) {
				std::abort();
			}

			font_array.emplace_back();
			auto content = simple_fs::view_contents(*ff);
			load_font(font_array.back(), content.data, content.file_size);
			font_array.back().only_raw_codepoints = state.user_settings.use_classic_fonts;
			font_array.back().file_name = fname;
			resolved = &(font_array.back());
		}

		state.world.locale_set_resolved_body_font(l, count);
	}
	{
		auto f = state.world.locale_get_header_font(l);
		std::string fname((char const*)f.begin(), (char const*)f.end());
		font* resolved = nullptr;
		uint16_t count = 0;

		for(auto& fnt : font_array) {
			if(fnt.file_name == fname) {
				resolved = &fnt;
				break;
			}
			++count;
		}

		if(!resolved) {
			auto r = simple_fs::get_root(state.common_fs);
			auto assets = simple_fs::open_directory(r, NATIVE("assets"));
			auto fonts = simple_fs::open_directory(assets, NATIVE("fonts"));
			auto ff = simple_fs::open_file(fonts, simple_fs::utf8_to_native(fname));
			if(!ff) {
				std::abort();
			}

			font_array.emplace_back();
			auto content = simple_fs::view_contents(*ff);
			load_font(font_array.back(), content.data, content.file_size);
			font_array.back().only_raw_codepoints = state.user_settings.use_classic_fonts;
			font_array.back().file_name = fname;
			resolved = &(font_array.back());
		}

		state.world.locale_set_resolved_header_font(l, count);
	}
	{
		auto f = state.world.locale_get_map_font(l);
		std::string fname((char const*)f.begin(), (char const*)f.end());
		font* resolved = nullptr;
		uint16_t count = 0;

		for(auto& fnt : font_array) {
			if(fnt.file_name == fname) {
				resolved = &fnt;
				break;
			}
			++count;
		}

		if(!resolved) {
			auto r = simple_fs::get_root(state.common_fs);
			auto assets = simple_fs::open_directory(r, NATIVE("assets"));
			auto fonts = simple_fs::open_directory(assets, NATIVE("fonts"));
			auto ff = simple_fs::open_file(fonts, simple_fs::utf8_to_native(fname));
			if(!ff) {
				std::abort();
			}

			font_array.emplace_back();
			auto content = simple_fs::view_contents(*ff);
			load_font(font_array.back(), content.data, content.file_size);
			font_array.back().only_raw_codepoints = state.user_settings.use_classic_fonts;
			font_array.back().file_name = fname;
			resolved = &(font_array.back());
		}

		state.world.locale_set_resolved_map_font(l, count);
	}

	state.reset_locale_pool();

	auto fb_name = state.world.locale_get_fallback(l);
	if(fb_name.size() > 0) {
		std::string_view fb_name_sv((char const*)fb_name.begin(), fb_name.size());
		state.load_locale_strings(fb_name_sv);
	}

	UErrorCode errorCode = U_ZERO_ERROR;
	UBreakIterator* lb_it = ubrk_open(UBreakIteratorType::UBRK_LINE, lang_str.c_str(), nullptr, 0, &errorCode);
	if(!lb_it || !U_SUCCESS(errorCode)) {
		std::abort(); // couldn't create iterator
	}
	auto rule_size = ubrk_getBinaryRules(lb_it, nullptr, 0, &errorCode);
	if(rule_size == 0 || !U_SUCCESS(errorCode)) {
		std::abort(); // couldn't get_rules
	}
	
	state.font_collection.compiled_ubrk_rules.resize(uint32_t(rule_size));
	ubrk_getBinaryRules(lb_it, state.font_collection.compiled_ubrk_rules.data(), rule_size, &errorCode);

	ubrk_close(lb_it);

	state.load_locale_strings(localename_sv);
}

font& font_manager::get_font(sys::state& state, font_selection s) {
	if(!current_locale)
		std::abort();
	switch(s) {
	case font_selection::body_font:
	default:
		return font_array[state.world.locale_get_resolved_body_font(current_locale)];
	case font_selection::header_font:
		return font_array[state.world.locale_get_resolved_header_font(current_locale)];
	case font_selection::map_font:
		return font_array[state.world.locale_get_resolved_map_font(current_locale)];
	}

}

void font_manager::load_font(font& fnt, char const* file_data, uint32_t file_size) {
	fnt.file_data = std::unique_ptr<FT_Byte[]>(new FT_Byte[file_size]);

	memcpy(fnt.file_data.get(), file_data, file_size);
	FT_New_Memory_Face(ft_library, fnt.file_data.get(), file_size, 0, &fnt.font_face);
	FT_Select_Charmap(fnt.font_face, FT_ENCODING_UNICODE);
	FT_Set_Pixel_Sizes(fnt.font_face, dr_size, dr_size);
	fnt.hb_font_face = hb_ft_font_create(fnt.font_face, nullptr);
	fnt.hb_buf = hb_buffer_create();

	fnt.internal_line_height = float(fnt.font_face->size->metrics.height) / float((1 << 6) * magnification_factor);
	fnt.internal_ascender = float(fnt.font_face->size->metrics.ascender) / float((1 << 6) * magnification_factor);
	fnt.internal_descender = -float(fnt.font_face->size->metrics.descender) / float((1 << 6) * magnification_factor);
	fnt.internal_top_adj = (fnt.internal_line_height - (fnt.internal_ascender + fnt.internal_descender)) / 2.0f;
}

float font::line_height(int32_t size) const {
	return internal_line_height * size / 64.0f;
}
float font::ascender(int32_t size) const {
	return internal_ascender * size / 64.0f;
}
float font::descender(int32_t size) const {
	return internal_descender * size / 64.0f;
}
float font::top_adjustment(int32_t size) const {
	return internal_top_adj * size / 64.0f;
}

bool font::can_display(char32_t ch_in) const {
	return FT_Get_Char_Index(font_face, ch_in) != 0;
}

float font::base_glyph_width(char32_t ch_in) {
	if(auto it = glyph_positions.find(ch_in); it != glyph_positions.end())
		return it->second.x_advance;

	make_glyph(ch_in);
	return glyph_positions[ch_in].x_advance;
}
void font::make_glyph(char32_t ch_in) {
	if(glyph_positions.find(ch_in) != glyph_positions.end())
		return;

	// load all glyph metrics
	if(ch_in) {
		FT_Load_Glyph(font_face, ch_in, FT_LOAD_TARGET_NORMAL | FT_LOAD_RENDER);

		FT_Glyph g_result;
		auto err = FT_Get_Glyph(font_face->glyph, &g_result);
		if(err != 0) {
			glyph_sub_offset gso;
			gso.x = 0.f;
			gso.y = 0.f;
			gso.x_advance = 0.f;
			gso.texture_slot = 0;
			glyph_positions.insert_or_assign(ch_in, gso);
			return;
		}

		FT_Bitmap const& bitmap = ((FT_BitmapGlyphRec*)g_result)->bitmap;

		float const hb_x = float(font_face->glyph->metrics.horiBearingX) / 64.f;
		float const hb_y = float(font_face->glyph->metrics.horiBearingY) / 64.f;

		
		uint8_t pixel_buffer[64 * 64] = { 0 };
		int const btmap_x_off = 32 * magnification_factor - bitmap.width / 2;
		int const btmap_y_off = 32 * magnification_factor - bitmap.rows / 2;

		glyph_sub_offset gso;
		gso.x = (hb_x - float(btmap_x_off)) * 1.0f / float(magnification_factor);
		gso.y = (-hb_y - float(btmap_y_off)) * 1.0f / float(magnification_factor);
		gso.x_advance = float(font_face->glyph->metrics.horiAdvance) / float((1 << 6) * magnification_factor);

		bool in_map[dr_size * dr_size] = {false};
		float distance_map[dr_size * dr_size] = {0.0f};
		init_in_map(in_map, bitmap.buffer, btmap_x_off, btmap_y_off, bitmap.width, bitmap.rows, uint32_t(bitmap.pitch));
		dead_reckoning(distance_map, in_map);
		for(int y = 0; y < 64; ++y) {
			for(int x = 0; x < 64; ++x) {
				const size_t index = size_t(x + y * 64);
				float const distance_value = distance_map[(x * magnification_factor + magnification_factor / 2) + (y * magnification_factor + magnification_factor / 2) * dr_size] / float(magnification_factor * 64);
				int const int_value = int(distance_value * -255.0f + 128.0f);
				const uint8_t small_value = uint8_t(std::min(255, std::max(0, int_value)));
				pixel_buffer[index] = small_value;
			}
		}
		//The array
		gso.texture_slot = first_free_slot;
		GLuint texid = 0;
		if((first_free_slot & 63) == 0) {
			GLuint new_text = 0;
			glGenTextures(1, &texid);
			glBindTexture(GL_TEXTURE_2D, texid);
			glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, 64 * 8, 64 * 8);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			textures.push_back(texid);
			uint32_t clearvalue = 0;
			glClearTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, &clearvalue);
		} else {
			assert(textures.size() > 0);
			texid = textures.back();
			assert(texid);
			glBindTexture(GL_TEXTURE_2D, texid);
		}
		if(texid) {
			auto sub_index = first_free_slot & 63;
			glTexSubImage2D(GL_TEXTURE_2D, 0, (sub_index & 7) * 64, ((sub_index >> 3) & 7) * 64, 64, 64, GL_RED, GL_UNSIGNED_BYTE, pixel_buffer);
		}
		FT_Done_Glyph(g_result);
		//after texture slot
		glyph_positions.insert_or_assign(ch_in, gso);
		++first_free_slot;
	}
}

stored_glyphs::stored_glyphs(sys::state& state, font_selection type, std::string const& s) {
	state.font_collection.get_font(state, type).remake_cache(state, type, *this, s);
}
stored_glyphs::stored_glyphs(std::string const& s, font& f) {
	f.remake_cache(*this, s);
}

stored_glyphs::stored_glyphs(sys::state& state, font_selection type, std::span<uint16_t> s) {
	state.font_collection.get_font(state, type).remake_cache(state, type, *this, s);
}
stored_glyphs::stored_glyphs(sys::state& state, font_selection type, std::span<uint16_t> s, no_bidi) {
	state.font_collection.get_font(state, type).remake_bidiless_cache(state, type, *this, s);
}

void stored_glyphs::set_text(sys::state& state, font_selection type, std::string const& s) {
	state.font_collection.get_font(state, type).remake_cache(state, type, *this, s);
}

stored_glyphs::stored_glyphs(stored_glyphs& other, uint32_t offset, uint32_t count) {
	glyph_info.resize(count);
	std::copy_n(other.glyph_info.data() + offset, count, glyph_info.data());
}

void font::remake_cache(sys::state& state, font_selection type, stored_glyphs& txt, std::span<uint16_t> source) {
	txt.glyph_info.clear();

	if(source.size() == 0)
		return;

	if(only_raw_codepoints && type != font_selection::map_font) {
		for(uint32_t i = 0; i < uint32_t(source.size()); i++) {
			text::stored_glyph glyph;
			glyph.codepoint = source[i];
			glyph.cluster = i;
			txt.glyph_info.push_back(glyph);
		}
		return;
	}

	auto locale = state.font_collection.get_current_locale();
	UBiDi* para;
	UErrorCode errorCode = U_ZERO_ERROR;

	para = ubidi_open();
	//para = ubidi_openSized(int32_t(temp_text.size()), 64, pErrorCode);
	if(!para)
		std::abort();

	hb_feature_t feature_buffer[10];
	auto features = type == font_selection::body_font ? state.world.locale_get_body_font_features(locale)
		: type == font_selection::header_font ? state.world.locale_get_header_font_features(locale)
		: state.world.locale_get_map_font_features(locale);
	for(uint32_t i = 0; i < uint32_t(std::extent_v<decltype(feature_buffer)>) && i < features.size(); ++i) {
		feature_buffer[i].tag = features[i];
		feature_buffer[i].start = 0;
		feature_buffer[i].end = (unsigned int)-1;
		feature_buffer[i].value = 1;
	}
	uint32_t hb_feature_count = std::min(features.size(), uint32_t(std::extent_v<decltype(feature_buffer)>));

	ubidi_setPara(para, (UChar const*)(source.data()), int32_t(source.size()), state.world.locale_get_native_rtl(locale) ? 1 : 0, nullptr, &errorCode);

	if(U_SUCCESS(errorCode)) {
		auto runcount = ubidi_countRuns(para, &errorCode);
		if(U_SUCCESS(errorCode)) {
			for(int32_t i = 0; i < runcount; ++i) {
				int32_t logical_start = 0;
				int32_t length = 0;
				auto direction = ubidi_getVisualRun(para, i, &logical_start, &length);

				// shape run with harfbuzz
				hb_buffer_clear_contents(hb_buf);
				hb_buffer_add_utf16(hb_buf, source.data(), int32_t(source.size()), logical_start, length);

				hb_buffer_set_direction(hb_buf, direction == UBIDI_RTL ? HB_DIRECTION_RTL : HB_DIRECTION_LTR);
				hb_buffer_set_script(hb_buf, (hb_script_t)state.world.locale_get_hb_script(locale));
				hb_buffer_set_language(hb_buf, state.world.locale_get_resolved_language(locale));

				hb_shape(hb_font_face, hb_buf, feature_buffer, hb_feature_count);

				uint32_t gcount = 0;
				hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(hb_buf, &gcount);
				hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(hb_buf, &gcount);

				for(unsigned int j = 0; j < gcount; j++) { // Preload glyphs
					make_glyph(glyph_info[j].codepoint);
					txt.glyph_info.emplace_back(glyph_info[j], glyph_pos[j]);
				}
			}
		} else {
			// failure to get number of runs
			std::abort();
		}
	} else {
		// failure to add text
		std::abort();
	}

	ubidi_close(para);
}

void font::remake_bidiless_cache(sys::state& state, font_selection type, stored_glyphs& txt, std::span<uint16_t> source) {
	txt.glyph_info.clear();
	if(source.size() == 0)
		return;

	if(only_raw_codepoints && type != font_selection::map_font) {
		for(uint32_t i = 0; i < uint32_t(source.size()); i++) {
			text::stored_glyph glyph;
			glyph.codepoint = source[i];
			glyph.cluster = i;
			txt.glyph_info.push_back(glyph);
		}
		return;
	}

	auto locale = state.font_collection.get_current_locale();
	
	hb_feature_t feature_buffer[10];
	auto features = type == font_selection::body_font ? state.world.locale_get_body_font_features(locale)
		: type == font_selection::header_font ? state.world.locale_get_header_font_features(locale)
		: state.world.locale_get_map_font_features(locale);
	for(uint32_t i = 0; i < uint32_t(std::extent_v<decltype(feature_buffer)>) && i < features.size(); ++i) {
		feature_buffer[i].tag = features[i];
		feature_buffer[i].start = 0;
		feature_buffer[i].end = (unsigned int)-1;
		feature_buffer[i].value = 1;
	}
	uint32_t hb_feature_count = std::min(features.size(), uint32_t(std::extent_v<decltype(feature_buffer)>));

	// shape run with harfbuzz
	hb_buffer_clear_contents(hb_buf);
	hb_buffer_add_utf16(hb_buf, source.data(), int32_t(source.size()), 0, int32_t(source.size()));

	hb_buffer_set_direction(hb_buf, state.world.locale_get_native_rtl(locale) ? HB_DIRECTION_RTL : HB_DIRECTION_LTR);
	hb_buffer_set_script(hb_buf, (hb_script_t)state.world.locale_get_hb_script(locale));
	hb_buffer_set_language(hb_buf, state.world.locale_get_resolved_language(locale));

	hb_shape(hb_font_face, hb_buf, feature_buffer, hb_feature_count);

	uint32_t gcount = 0;
	hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(hb_buf, &gcount);
	hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(hb_buf, &gcount);

	for(unsigned int j = 0; j < gcount; j++) { // Preload glyphs
		make_glyph(glyph_info[j].codepoint);
		txt.glyph_info.emplace_back(glyph_info[j], glyph_pos[j]);
	}

	if(state.world.locale_get_native_rtl(locale)) {
		std::reverse(txt.glyph_info.begin(), txt.glyph_info.end());
	}
}

void font::remake_cache(stored_glyphs& txt, std::string const& s) {
	txt.glyph_info.clear();
	if(s.length() == 0)
		return;

	if(only_raw_codepoints) {
		for(uint32_t i = 0; i < uint32_t(s.size());) {
			text::stored_glyph glyph;
			glyph.codepoint = text::codepoint_from_utf8(s.data() + i, s.data() + s.size());
			glyph.cluster = i;
			txt.glyph_info.push_back(glyph);
			i += uint32_t(text::size_from_utf8(s.data() + i, s.data() + s.size()));
		}
		return;
	}

	hb_buffer_clear_contents(hb_buf);
	hb_buffer_add_utf8(hb_buf, s.c_str(), int(s.length()), 0, int(s.length()));

	hb_buffer_set_direction(hb_buf, HB_DIRECTION_LTR);
	hb_buffer_set_script(hb_buf, HB_SCRIPT_LATIN);
	hb_buffer_set_language(hb_buf, hb_language_from_string("en", -1));

	hb_feature_t feature_buffer[10];
	hb_shape(hb_font_face, hb_buf, feature_buffer, 0);

	uint32_t gcount = 0;
	hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(hb_buf, &gcount);
	hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(hb_buf, &gcount);
	for(unsigned int i = 0; i < gcount; i++) { // Preload glyphs
		make_glyph(glyph_info[i].codepoint);
		txt.glyph_info.emplace_back(glyph_info[i], glyph_pos[i]);
	}
}

void font::remake_cache(sys::state& state, font_selection type, stored_glyphs& txt, std::string const& s) {
	txt.glyph_info.clear();
	if(s.length() == 0)
		return;

	if(only_raw_codepoints && type != font_selection::map_font) {
		for(uint32_t i = 0; i < uint32_t(s.size());) {
			text::stored_glyph glyph;
			glyph.codepoint = text::codepoint_from_utf8(s.data() + i, s.data() + s.size());
			glyph.cluster = i;
			txt.glyph_info.push_back(glyph);
			i += uint32_t(text::size_from_utf8(s.data() + i, s.data() + s.size()));
		}
		return;
	}

	auto locale = state.font_collection.get_current_locale();
	if(state.world.locale_get_native_rtl(locale) == false) {
		hb_buffer_clear_contents(hb_buf);
		hb_buffer_add_utf8(hb_buf, s.c_str(), int(s.length()), 0, int(s.length()));

		hb_buffer_set_direction(hb_buf, HB_DIRECTION_LTR);
		hb_buffer_set_script(hb_buf, (hb_script_t)state.world.locale_get_hb_script(locale));
		hb_buffer_set_language(hb_buf, state.world.locale_get_resolved_language(locale));

		hb_feature_t feature_buffer[10];
		auto features = type == font_selection::body_font ? state.world.locale_get_body_font_features(locale)
			: type == font_selection::header_font ? state.world.locale_get_header_font_features(locale)
			: state.world.locale_get_map_font_features(locale);
		for(uint32_t i = 0; i < uint32_t(std::extent_v<decltype(feature_buffer)>) && i < features.size(); ++i) {
			feature_buffer[i].tag = features[i];
			feature_buffer[i].start = 0;
			feature_buffer[i].end = (unsigned int)-1;
			feature_buffer[i].value = 1;
		}
		uint32_t hb_feature_count = std::min(features.size(), uint32_t(std::extent_v<decltype(feature_buffer)>));
		hb_shape(hb_font_face, hb_buf, feature_buffer, hb_feature_count);

		uint32_t gcount = 0;
		hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(hb_buf, &gcount);
		hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(hb_buf, &gcount);
		for(unsigned int i = 0; i < gcount; i++) { // Preload glyphs
			make_glyph(glyph_info[i].codepoint);
			txt.glyph_info.emplace_back(glyph_info[i], glyph_pos[i]);
		}
	} else {
		std::vector<uint16_t> temp_text;
		std::vector<uint16_t> to_base_char;

		auto start = s.c_str();
		auto end = start + s.length();
		int32_t base_index = 0;
		while(start + base_index < end) {
			auto c = text::codepoint_from_utf8(start + base_index, end);
			if(!requires_surrogate_pair(c)) {
				temp_text.push_back(char16_t(c));
				to_base_char.push_back(uint16_t(base_index));
			} else {
				auto p = make_surrogate_pair(c);
				temp_text.push_back(char16_t(p.high));
				temp_text.push_back(char16_t(p.low));
				to_base_char.push_back(uint16_t(base_index));
				to_base_char.push_back(uint16_t(base_index));
			}
			base_index += int32_t(size_from_utf8(start + base_index, end));
		}

		UErrorCode errorCode = U_ZERO_ERROR;
		UBiDi* para = ubidi_open();
		//para = ubidi_openSized(int32_t(temp_text.size()), 64, pErrorCode);
		if(!para)
			std::abort();

		hb_feature_t feature_buffer[10];
		auto features = type == font_selection::body_font ? state.world.locale_get_body_font_features(locale)
			: type == font_selection::header_font ? state.world.locale_get_header_font_features(locale)
			: state.world.locale_get_map_font_features(locale);
		for(uint32_t i = 0; i < uint32_t(std::extent_v<decltype(feature_buffer)>) && i < features.size(); ++i) {
			feature_buffer[i].tag = features[i];
			feature_buffer[i].start = 0;
			feature_buffer[i].end = (unsigned int)-1;
			feature_buffer[i].value = 1;
		}
		uint32_t hb_feature_count = std::min(features.size(), uint32_t(std::extent_v<decltype(feature_buffer)>));

		ubidi_setPara(para, (UChar const*)temp_text.data(), int32_t(temp_text.size()), 1, nullptr, &errorCode);

		if(U_SUCCESS(errorCode)) {
			auto runcount = ubidi_countRuns(para, &errorCode);
			if(U_SUCCESS(errorCode)) {
				for(int32_t i = 0; i < runcount; ++i) {
					int32_t logical_start = 0;
					int32_t length = 0;
					auto direction = ubidi_getVisualRun(para, i, &logical_start, &length);

					// shape run with harfbuzz
					hb_buffer_clear_contents(hb_buf);
					hb_buffer_add_utf16(hb_buf, temp_text.data(), int32_t(temp_text.size()), logical_start, length);

					hb_buffer_set_direction(hb_buf, direction == UBIDI_RTL ? HB_DIRECTION_RTL : HB_DIRECTION_LTR);
					hb_buffer_set_script(hb_buf, (hb_script_t)state.world.locale_get_hb_script(locale));
					hb_buffer_set_language(hb_buf, state.world.locale_get_resolved_language(locale));

					hb_shape(hb_font_face, hb_buf, feature_buffer, hb_feature_count);

					uint32_t gcount = 0;
					hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(hb_buf, &gcount);
					hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(hb_buf, &gcount);

					for(unsigned int j = 0; j < gcount; j++) { // Preload glyphs
						make_glyph(glyph_info[j].codepoint);
						txt.glyph_info.emplace_back(glyph_info[j], glyph_pos[j]);
						txt.glyph_info.back().cluster = to_base_char[glyph_info[j].cluster];
					}
				}
			} else {
				// failure to get number of runs
				std::abort();
			}
		} else {
			// failure to add text
			std::abort();
		}

		ubidi_close(para);
	}
}

float font::text_extent(sys::state& state, stored_glyphs const& txt, uint32_t starting_offset, uint32_t count, int32_t size) {
	float x_total = 0.0f;
	for(uint32_t i = starting_offset; i < starting_offset + count; i++) {
		hb_codepoint_t glyphid = txt.glyph_info[i].codepoint;
		float x_advance = float(txt.glyph_info[i].x_advance) / (float((1 << 6) * text::magnification_factor));
		x_total += x_advance * size / 64.f;
	}
	return x_total;
}

void font_manager::set_classic_fonts(bool v) {
	for(auto& fnt : font_array) {
		fnt.only_raw_codepoints = v;
	}
}

} // namespace text

