#include <cmath>
#include <bit>

extern "C" {
#include "hb.h"
#include "hb-ft.h"
#include "fribidi.h"
}

#include "fonts.hpp"
#include "parsers.hpp"
#include "simple_fs.hpp"
#include "system_state.hpp"

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
			load_font(font_array.back(), content.data, content.file_size );
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

	state.load_locale_strings(localename_sv);
}

font& font_manager::get_font(sys::state& state, font_selection s) {
	if(!current_locale)
		std::abort();
	switch(s) {
	case font_selection::body_font:
		return font_array[state.world.locale_get_resolved_body_font(current_locale)];
	case font_selection::header_font:
		return font_array[state.world.locale_get_resolved_header_font(current_locale)];
	case font_selection::map_font:
		return font_array[state.world.locale_get_resolved_map_font(current_locale)];
	default:
		return font_array[state.world.locale_get_resolved_body_font(current_locale)];
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

std::string font::get_conditional_indicator(bool v) const {
	if(v) {
		return "@(T)";
	} else {
		return "@(F)";
	}
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

		auto sub_index = ch_in & 63;
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
		gso.texture_slot = uint16_t(ch_in >> 6);
		GLuint texid = 0;
		if(auto it = texture_slots.find(gso.texture_slot); it != texture_slots.end()) {
			texid = it->second;
			glBindTexture(GL_TEXTURE_2D, texid);
		} else {
			glGenTextures(1, &texid);
			glBindTexture(GL_TEXTURE_2D, texid);
			glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, 64 * 8, 64 * 8);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			texture_slots.insert_or_assign(gso.texture_slot, texid);
		}
		if(texid) {
			glTexSubImage2D(GL_TEXTURE_2D, 0, (sub_index & 7) * 64, ((sub_index >> 3) & 7) * 64, 64, 64, GL_RED, GL_UNSIGNED_BYTE, pixel_buffer);
		}
		FT_Done_Glyph(g_result);
		//after texture slot
		glyph_positions.insert_or_assign(ch_in, gso);
	}
}

char font::codepoint_to_alnum(char32_t codepoint) {
	std::string_view alnum_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789()";
	for(const auto c : alnum_table)
		if(codepoint == FT_Get_Char_Index(font_face, c))
			return c;
	return 0;
}

stored_text::stored_text(sys::state& state, font_selection type, std::string const& s) : stored_glyphs(state, type, s), base_text(s) {
}
stored_text::stored_text(sys::state& state, font_selection type, std::string&& s) : stored_glyphs(state, type, s), base_text(std::move(s)) {
}

stored_glyphs::stored_glyphs(sys::state& state, font_selection type, std::string const& s) {
	state.font_collection.get_font(state, type).remake_cache(state, type, *this, s);
}

void stored_glyphs::set_text(sys::state& state, font_selection type, std::string const& s) {
	state.font_collection.get_font(state, type).remake_cache(state, type, *this, s);
}

stored_glyphs::stored_glyphs(stored_glyphs& other, uint32_t offset, uint32_t count) {
	glyph_count = count;
	glyph_info.resize(count);
	glyph_pos.resize(count);
	std::copy_n(other.glyph_info.data() + offset, count, glyph_info.data());
	std::copy_n(other.glyph_pos.data() + offset, count, glyph_pos.data());
}

void stored_text::set_text(sys::state& state, font_selection type, std::string const& s) {
	if(base_text != s) {
		base_text = s;
		stored_glyphs::set_text(state, type, s);
	}
}
void stored_text::set_text(sys::state& state, font_selection type, std::string&& s) {
	if(base_text != s) {
		stored_glyphs::set_text(state, type, s);
		base_text = std::move(s);
	}
}

void font::remake_cache(sys::state& state, font_selection type, stored_glyphs& txt, std::string const& str) {
	hb_buffer_clear_contents(hb_buf);

	auto visual_str = std::unique_ptr<FriBidiChar>(new FriBidiChar[str.size() + 1]);
	auto pos_l_to_v = std::unique_ptr<FriBidiStrIndex>(new FriBidiStrIndex[str.size() + 1]);
	auto pos_v_to_l = std::unique_ptr<FriBidiStrIndex>(new FriBidiStrIndex[str.size() + 1]);
	auto emb_level_list = std::unique_ptr<FriBidiLevel>(new FriBidiLevel[str.size() + 1]);
	FriBidiCharType pbase_dir = FRIBIDI_TYPE_ON;
	fribidi_log2vis((const FriBidiChar*)str.c_str(), FriBidiStrIndex(str.size()), &pbase_dir, visual_str.get(), pos_l_to_v.get(), pos_v_to_l.get(), emb_level_list.get());

	txt.glyph_count = 0;
	{
		auto s = std::string((const char*)visual_str.get());
		hb_buffer_add_utf8(hb_buf, s.c_str(), int(s.length()), 0, int(s.length()));

		auto locale = state.font_collection.get_current_locale();
		hb_buffer_set_direction(hb_buf, FRIBIDI_IS_RTL(pbase_dir) ? HB_DIRECTION_RTL : HB_DIRECTION_LTR);
		hb_buffer_set_script(hb_buf, (hb_script_t)state.world.locale_get_hb_script(locale));
		hb_buffer_set_language(hb_buf, state.world.locale_get_resolved_language(locale));

		hb_feature_t feature_buffer[10];
		dcon::dcon_vv_fat_id<uint32_t> features = type == font_selection::header_font ? state.world.locale_get_header_font_features(locale)
			: type == font_selection::map_font ? state.world.locale_get_map_font_features(locale)
			: type == font_selection::body_font ? state.world.locale_get_body_font_features(locale)
			: state.world.locale_get_body_font_features(locale);
		for(uint32_t i = 0; i < uint32_t(std::extent_v<decltype(feature_buffer)>) && i < features.size(); ++i) {
			feature_buffer[i].tag = features[i];
			feature_buffer[i].start = 0;
			feature_buffer[i].end = (unsigned int)-1;
			feature_buffer[i].value = 1;
		}
		hb_shape(hb_font_face, hb_buf, feature_buffer, std::min(features.size(), uint32_t(std::extent_v<decltype(feature_buffer)>)));

		unsigned int glyph_count = 0;
		hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(hb_buf, &glyph_count);
		hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(hb_buf, &glyph_count);
		auto glyph_offset = txt.glyph_count;
		txt.glyph_count += glyph_count;
		txt.glyph_info.resize(size_t(txt.glyph_count));
		std::memcpy(txt.glyph_info.data() + glyph_offset, glyph_info, glyph_count * sizeof(glyph_info[0]));
		txt.glyph_pos.resize(size_t(txt.glyph_count));
		std::memcpy(txt.glyph_pos.data() + glyph_offset, glyph_pos, glyph_count * sizeof(glyph_pos[0]));
	}

	// Preload all glyphs
	for(unsigned int i = 0; i < txt.glyph_count; i++) {
		make_glyph(txt.glyph_info[i].codepoint);
	}
}

float font::text_extent(sys::state& state, stored_glyphs const& txt, uint32_t starting_offset, uint32_t count, int32_t size) {
	float x_total = 0.0f;

	hb_glyph_position_t const* glyph_pos = txt.glyph_pos.data() + starting_offset;
	hb_glyph_info_t const* glyph_info = txt.glyph_info.data() + starting_offset;
	unsigned int glyph_count = static_cast<unsigned int>(count);
	for(unsigned int i = 0; i < glyph_count; i++) {
		hb_codepoint_t glyphid = glyph_info[i].codepoint;
		make_glyph(glyphid);
		auto gso = glyph_positions[glyphid];
		float x_advance = float(glyph_pos[i].x_advance) / (float((1 << 6) * text::magnification_factor));
		bool draw_icon = false;
		bool draw_flag = false;
		if(glyphid == FT_Get_Char_Index(font_face, '@')) {
			char tag[3] = { 0, 0, 0 };
			tag[0] = (i + 1 < glyph_count) ? codepoint_to_alnum(glyph_info[i + 1].codepoint) : 0;
			tag[1] = (i + 2 < glyph_count) ? codepoint_to_alnum(glyph_info[i + 2].codepoint) : 0;
			tag[2] = (i + 3 < glyph_count) ? codepoint_to_alnum(glyph_info[i + 3].codepoint) : 0;
			if(tag[0] == '(' && tag[2] == ')') {
				draw_icon = true;
			} else if(tag[0] != 0 && tag[1] != 0 && tag[2] != 0) {
				draw_flag = true;
				draw_icon = true;
			}
			if(draw_icon) {
				i += 3;
			}
		}
		x_total += x_advance * (draw_flag ? 1.5f : 1.f) * size / 64.f;
	}

	return x_total;
}

} // namespace text

