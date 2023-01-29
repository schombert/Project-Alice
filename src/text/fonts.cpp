#include <cmath>

#include "fonts.hpp"
#include "parsers.hpp"

namespace text {

constexpr uint16_t pack_font_handle(uint32_t font_index, bool black, uint32_t size) {
	return uint16_t(uint32_t(font_index << 9) | uint32_t(black ? (1 << 8) : 0) | uint32_t(size & 0xFF));
}


bool is_black_font(std::string_view txt) {
	if(parsers::has_fixed_suffix_ci(txt.data(), txt.data() + txt.length(), "_bl")
		|| parsers::has_fixed_suffix_ci(txt.data(), txt.data() + txt.length(), "black")
		|| parsers::has_fixed_suffix_ci(txt.data(), txt.data() + txt.length(), "black_bold")
		) {
		return true;
	} else {
		return false;
	}
}

uint32_t font_size(std::string_view txt) {
	const char* first_int = txt.data();
	const char* end = txt.data() + txt.size();
	while(first_int != end) {
		if(isdigit(*first_int))
			break;
		++first_int;
	}
	const char* last_int = first_int;
	while(last_int != end) {
		if(!isdigit(*last_int))
			break;
		++last_int;
	}
	if(first_int == last_int) {
		if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "fps_font"))
			return 14;
		else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "tooltip_font"))
			return 16;
		else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "frangoth_bold"))
			return 18;
		else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "impact_small"))
			return 24;
		else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "old_english"))
			return 50;
		else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "timefont"))
			return 24;
		else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "vic_title"))
			return 42;
		else
			return 14;
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

uint16_t name_into_font_id(std::string_view txt) {
	return pack_font_handle(font_index(txt), is_black_font(txt), font_size(txt));
}

int32_t size_from_font_id(uint16_t id) {
	return int32_t(id & 0xFF);
}

bool is_black_from_font_id(uint16_t id) {
	return ((id >> 8) & 1) != 0;
}
uint32_t font_index_from_font_id(uint16_t id) {
	return uint32_t((id >> 9) & 0xFF);
}

font_manager::font_manager() {
	FT_Init_FreeType(&ft_library);
}
font_manager::~font_manager() {
	FT_Done_FreeType(ft_library);
}

font::~font() {
	//if(loaded)
	//	FT_Done_Face(font_face);
}


int32_t transform_offset_b(int32_t x, int32_t y, int32_t btmap_x_off, int32_t btmap_y_off, uint32_t width, uint32_t height, uint32_t pitch) {
	int bmp_x = x - btmap_x_off;
	int bmp_y = y - btmap_y_off;

	if((bmp_x < 0) | (bmp_x >= (int32_t)width) | (bmp_y < 0) | (bmp_y >= (int32_t)height))
		return -1;
	else
		return bmp_x + bmp_y * (int32_t)pitch;
}


constexpr int magnification_factor = 4;
constexpr int dr_size = 64 * magnification_factor;
constexpr float rt_2 = 1.41421356237309504f;

void init_in_map(bool in_map[dr_size * dr_size], uint8_t* bmp_data, int32_t btmap_x_off, int32_t btmap_y_off, uint32_t width, uint32_t height, uint32_t pitch) {
	for(int32_t j = 0; j < dr_size; ++j) {
		for(int32_t i = 0; i < dr_size; ++i) {
			const auto boff = transform_offset_b(i, j, btmap_x_off, btmap_y_off, width, height, pitch);
			in_map[i + dr_size * j] = (boff != -1) ? (bmp_data[boff] > 127) : false;
		}
	}
}

//
// based on The "dead reckoning" signed distance transform
// Grevera, George J. (2004) Computer Vision and Image Understanding 95 pages 317–333
//

void dead_reckoning(float distance_map[dr_size * dr_size], const bool in_map[dr_size * dr_size]) {
	int16_t yborder[dr_size * dr_size] = { 0 };
	int16_t xborder[dr_size * dr_size] = { 0 };

	for(uint32_t i = 0; i < dr_size * dr_size; ++i) {
		distance_map[i] = std::numeric_limits<float>::infinity();
	}
	for(int32_t j = 1; j < dr_size - 1; ++j) {
		for(int32_t i = 1; i < dr_size - 1; ++i) {
			if(in_map[i - 1 + dr_size * j] != in_map[i + dr_size * j] ||
				in_map[i + 1 + dr_size * j] != in_map[i + dr_size * j] ||
				in_map[i + dr_size * (j + 1)] != in_map[i + dr_size * j] ||
				in_map[i + dr_size * (j - 1)] != in_map[i + dr_size * j]) {
				distance_map[i + dr_size * j] = 0.0f;
				yborder[i + dr_size * j] = static_cast<int16_t>(j);
				xborder[i + dr_size * j] = static_cast<int16_t>(i);
			}
		}
	}
	for(int32_t j = 1; j < dr_size - 1; ++j) {
		for(int32_t i = 1; i < dr_size - 1; ++i) {
			if(distance_map[(i - 1) + dr_size * (j - 1)] + rt_2 < distance_map[(i)+dr_size * (j)]) {
				yborder[i + dr_size * j] = yborder[(i - 1) + dr_size * (j - 1)];
				xborder[i + dr_size * j] = xborder[(i - 1) + dr_size * (j - 1)];
				distance_map[(i)+dr_size * (j)] =
					(float)std::sqrt((i - xborder[i + dr_size * j]) * (i - xborder[i + dr_size * j]) +
					(j - yborder[i + dr_size * j]) * (j - yborder[i + dr_size * j]));
			}
			if(distance_map[(i)+dr_size * (j - 1)] + 1.0f < distance_map[(i)+dr_size * (j)]) {
				yborder[i + dr_size * j] = yborder[(i)+dr_size * (j - 1)];
				xborder[i + dr_size * j] = xborder[(i)+dr_size * (j - 1)];
				distance_map[(i)+dr_size * (j)] =
					(float)std::sqrt((i - xborder[i + dr_size * j]) * (i - xborder[i + dr_size * j]) +
					(j - yborder[i + dr_size * j]) * (j - yborder[i + dr_size * j]));
			}
			if(distance_map[(i + 1) + dr_size * (j - 1)] + rt_2 < distance_map[(i)+dr_size * (j)]) {
				yborder[i + dr_size * j] = yborder[(i + 1) + dr_size * (j - 1)];
				xborder[i + dr_size * j] = xborder[(i + 1) + dr_size * (j - 1)];
				distance_map[(i)+dr_size * (j)] =
					(float)std::sqrt((i - xborder[i + dr_size * j]) * (i - xborder[i + dr_size * j]) +
					(j - yborder[i + dr_size * j]) * (j - yborder[i + dr_size * j]));
			}
			if(distance_map[(i - 1) + dr_size * (j)] + 1.0f < distance_map[(i)+dr_size * (j)]) {
				yborder[i + dr_size * j] = yborder[(i - 1) + dr_size * (j)];
				xborder[i + dr_size * j] = xborder[(i - 1) + dr_size * (j)];
				distance_map[(i)+dr_size * (j)] =
					(float)std::sqrt((i - xborder[i + dr_size * j]) * (i - xborder[i + dr_size * j]) +
					(j - yborder[i + dr_size * j]) * (j - yborder[i + dr_size * j]));
			}
		}
	}

	for(int32_t j = dr_size - 2; j > 0; --j) {
		for(int32_t i = dr_size - 2; i > 0; --i) {
			if(distance_map[(i + 1) + dr_size * (j)] + 1.0f < distance_map[(i)+dr_size * (j)]) {
				yborder[i + dr_size * j] = yborder[(i + 1) + dr_size * (j)];
				xborder[i + dr_size * j] = xborder[(i + 1) + dr_size * (j)];
				distance_map[(i)+dr_size * (j)] =
					(float)std::sqrt((i - xborder[i + dr_size * j]) * (i - xborder[i + dr_size * j]) +
					(j - yborder[i + dr_size * j]) * (j - yborder[i + dr_size * j]));
			}
			if(distance_map[(i - 1) + dr_size * (j + 1)] + rt_2 < distance_map[(i)+dr_size * (j)]) {
				yborder[i + dr_size * j] = yborder[(i - 1) + dr_size * (j + 1)];
				xborder[i + dr_size * j] = xborder[(i - 1) + dr_size * (j + 1)];
				distance_map[(i)+dr_size * (j)] =
					(float)std::sqrt((i - xborder[i + dr_size * j]) * (i - xborder[i + dr_size * j]) +
					(j - yborder[i + dr_size * j]) * (j - yborder[i + dr_size * j]));
			}
			if(distance_map[(i)+dr_size * (j + 1)] + 1.0f < distance_map[(i)+dr_size * (j)]) {
				yborder[i + dr_size * j] = yborder[(i)+dr_size * (j + 1)];
				xborder[i + dr_size * j] = xborder[(i)+dr_size * (j + 1)];
				distance_map[(i)+dr_size * (j)] =
					(float)std::sqrt((i - xborder[i + dr_size * j]) * (i - xborder[i + dr_size * j]) +
					(j - yborder[i + dr_size * j]) * (j - yborder[i + dr_size * j]));
			}
			if(distance_map[(i + 1) + dr_size * (j + 1)] + rt_2 < distance_map[(i)+dr_size * (j)]) {
				yborder[i + dr_size * j] = yborder[(i + 1) + dr_size * (j + 1)];
				xborder[i + dr_size * j] = xborder[(i + 1) + dr_size * (j + 1)];
				distance_map[(i)+dr_size * (j)] =
					(float)std::sqrt((i - xborder[i + dr_size * j]) * (i - xborder[i + dr_size * j]) +
					(j - yborder[i + dr_size * j]) * (j - yborder[i + dr_size * j]));
			}
		}
	}

	for(uint32_t i = 0; i < dr_size * dr_size; ++i) {
		if(in_map[i])
			distance_map[i] *= -1.0f;
	}
}

void font_manager::load_font(font& fnt, char const* file_data, uint32_t file_size) {
	fnt.file_data = std::unique_ptr<FT_Byte[]>(new FT_Byte[file_size]);
	memcpy(fnt.file_data.get(), file_data, file_size);
	FT_New_Memory_Face(ft_library, fnt.file_data.get(), file_size, 0, &fnt.font_face);
	FT_Select_Charmap(fnt.font_face, FT_ENCODING_UNICODE);
	FT_Set_Pixel_Sizes(fnt.font_face, 0, 64 * magnification_factor);
	fnt.loaded = true;

	fnt.internal_line_height = static_cast<float>(fnt.font_face->size->metrics.height) / static_cast<float>((1 << 6) * magnification_factor);
	fnt.internal_ascender = static_cast<float>(fnt.font_face->size->metrics.ascender) / static_cast<float>((1 << 6) * magnification_factor);
	fnt.internal_descender = - static_cast<float>(fnt.font_face->size->metrics.descender) / static_cast<float>((1 << 6) * magnification_factor);
	fnt.internal_top_adj = (fnt.internal_line_height - (fnt.internal_ascender + fnt.internal_descender)) / 2.0f;

	// load all glyph metrics

	for(int32_t i = 0; i < 256; ++i) {
		const auto index_in_this_font = FT_Get_Char_Index(fnt.font_face, win1250toUTF16(char(i)));
		if(index_in_this_font) {
			FT_Load_Glyph(fnt.font_face, index_in_this_font, FT_LOAD_TARGET_NORMAL);
			fnt.glyph_advances[i] = static_cast<float>(fnt.font_face->glyph->metrics.horiAdvance) / static_cast<float>((1 << 6) * magnification_factor);
		}
	}
}

float font::kerning(char codepoint_first, char codepoint_second) const {
	auto utf16_first = win1250toUTF16(codepoint_first);
	auto utf16_second = win1250toUTF16(codepoint_second);
	const auto index_a = FT_Get_Char_Index(font_face, utf16_first);
	const auto index_b = FT_Get_Char_Index(font_face, utf16_second);

	if((index_a == 0) || (index_b == 0)) {
		return 0.0f;
	}

	if(FT_HAS_KERNING(font_face)) {
		FT_Vector kerning;
		FT_Get_Kerning(font_face, index_a, index_b, FT_KERNING_DEFAULT, &kerning);
		return static_cast<float>(kerning.x) / static_cast<float>((1 << 6) * magnification_factor);
	} else {
		return 0.0f;
	}
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


void font::make_glyph(char ch_in) {
	if(glyph_loaded[uint32_t(ch_in)])
		return;
	glyph_loaded[uint32_t(ch_in)] = true;

	auto codepoint = win1250toUTF16(ch_in);
	if(codepoint == ' ')
		return;

	const auto index_in_this_font = FT_Get_Char_Index(font_face, codepoint);
	if(index_in_this_font) {
		FT_Load_Glyph(font_face, index_in_this_font, FT_LOAD_TARGET_NORMAL | FT_LOAD_RENDER);

		FT_Glyph g_result;
		FT_Get_Glyph(font_face->glyph, &g_result);

		auto texture_number = uint32_t(ch_in) >> 6;

		if(textures[texture_number] == 0) {
			glGenTextures(1, &textures[texture_number]);
			glBindTexture(GL_TEXTURE_2D, textures[texture_number]);
			glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, 64 * 8, 64 * 8);

			//glClearTexImage(textures[texture_number], 0, GL_RED, GL_FLOAT, nullptr);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		} else {
			glBindTexture(GL_TEXTURE_2D, textures[texture_number]);
		}

		FT_Bitmap& bitmap = ((FT_BitmapGlyphRec*)g_result)->bitmap;

		const float hb_x = static_cast<float>(font_face->glyph->metrics.horiBearingX) / static_cast<float>(1 << 6);
		const float hb_y = static_cast<float>(font_face->glyph->metrics.horiBearingY) / static_cast<float>(1 << 6);

		auto sub_index = (uint32_t(ch_in) & 63);

		uint8_t pixel_buffer[64 * 64];

		const int btmap_x_off = 32 * magnification_factor - bitmap.width / 2;
		const int btmap_y_off = 32 * magnification_factor - bitmap.rows / 2;

		glyph_positions[uint32_t(ch_in)].x = (hb_x - static_cast<float>(btmap_x_off)) * 1.0f / static_cast<float>(magnification_factor);
		glyph_positions[uint32_t(ch_in)].y = (-hb_y - static_cast<float>(btmap_y_off)) * 1.0f / static_cast<float>(magnification_factor);

		bool in_map[dr_size * dr_size] = { false };
		float distance_map[dr_size * dr_size];

		init_in_map(in_map, bitmap.buffer, btmap_x_off, btmap_y_off, bitmap.width, bitmap.rows, (uint32_t)bitmap.pitch);
		dead_reckoning(distance_map, in_map);


		for(int y = 0; y < 64; ++y) {
			for(int x = 0; x < 64; ++x) {

				const size_t index = static_cast<size_t>(x + y * 64);
				const float distance_value = distance_map[
					(x * magnification_factor + magnification_factor / 2) +
						(y * magnification_factor + magnification_factor / 2) * dr_size]
					/ static_cast<float>(magnification_factor * 64);
					const int int_value = static_cast<int>(distance_value * -255.0f + 128.0f);
					const uint8_t small_value = static_cast<uint8_t>(std::min(255, std::max(0, int_value)));

					pixel_buffer[index] = small_value;
			}
		}

		glTexSubImage2D(GL_TEXTURE_2D, 0,
			(sub_index & 7) * 64,
			((sub_index >> 3) & 7) * 64,
			64,
			64,
			GL_RED, GL_UNSIGNED_BYTE, pixel_buffer);

		FT_Done_Glyph(g_result);
	}
}

float font::text_extent(const char* codepoints, uint32_t count, int32_t size) const {
	float total = 0.0f;
	for(; count-- > 0; ) {

		total +=
			this->glyph_advances[uint32_t(codepoints[count])] * size / 64.0f +
			((count != 0) ? kerning(codepoints[count - 1], codepoints[count]) * size / 64.0f : 0.0f);
	}
	return total;
}


void load_standard_fonts(sys::state& state) {
	auto root = get_root(state.common_fs);
	auto font_a = open_file(root, NATIVE("LibreCaslonText-Regular.ttf"));
	if(font_a) {
		auto file_content = view_contents(*font_a);
		state.font_collection.load_font(state.font_collection.fonts[0], file_content.data, file_content.file_size);
	}
	auto font_b = open_file(root, NATIVE("AndadaSC-Regular.otf"));
	if(font_b) {
		auto file_content = view_contents(*font_b);
		state.font_collection.load_font(state.font_collection.fonts[1], file_content.data, file_content.file_size);
	}
}

}

