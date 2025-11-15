#include "asvg.hpp"
#include "lunasvg.h"
#include <charconv>
#include "glew.h"
#include "system_state.hpp"

namespace asvg {

svg_instance::~svg_instance() noexcept {
	if(texture_handle != 0) {
		glDeleteTextures(1, &texture_handle);
		texture_handle = 0;
	}
}

svg_instance::svg_instance(char const* bytes, int32_t sx, int32_t sy) {
	glGenTextures(1, &texture_handle);
	if(texture_handle) {
		glBindTexture(GL_TEXTURE_2D, texture_handle);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, sx, sy);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, sx, sy, GL_RGBA, GL_UNSIGNED_BYTE, bytes);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

svg_instance::svg_instance(svg_instance&& other) noexcept {
	if(texture_handle != 0) {
		glDeleteTextures(1, &texture_handle);
	}
	texture_handle = other.texture_handle;
	other.texture_handle = 0;
}

svg_instance& svg_instance::operator=(svg_instance&& other) noexcept {
	if(texture_handle != 0) {
		glDeleteTextures(1, &texture_handle);
	}
	texture_handle = other.texture_handle;
	other.texture_handle = 0;
	return *this;
}

svg::svg(char const* data, size_t count, int32_t base_width, int32_t base_height) : svg_data(data, data+count), base_width(base_width), base_height(base_height) {
	for(size_t i = 0; i < count; ++i) {
		if(svg_data[i] == '[' && i + 1 < count && svg_data[i + 1] == '[') {
			affine_replacement new_rep{ };

			if(i > 0 && svg_data[i - 1] == '\"') {
				new_rep.emit_quotes = true;
				new_rep.start_position = uint32_t(i -1);
			} else {
				new_rep.start_position = uint32_t(i);
			}
			i += 2;

			int32_t stage = 0;
			while( i < count ) {
				if(svg_data[i] == ']' && i + 1 < count && svg_data[i + 1] == ']') {
					if(i + 2 < count && svg_data[i + 2] == '\"') {
						new_rep.emit_quotes = true;
						++i;
					}
					i += 2;
					new_rep.end_position = uint32_t(i);
					replacements.push_back(new_rep);
					break;
				} else if(stage == 0) { // read dimension
					if(svg_data[i] == 'W' || svg_data[i] == 'w' || svg_data[i] == 'X' || svg_data[i] == 'x') {
						new_rep.dimension = dimension_relative::width;
					} else if(svg_data[i] == 'H' || svg_data[i] == 'h' || svg_data[i] == 'Y' || svg_data[i] == 'y') {
						new_rep.dimension = dimension_relative::height;
					} else if(svg_data[i] == 'S' || svg_data[i] == 's') {
						new_rep.dimension = dimension_relative::smaller;
					} else if(svg_data[i] == 'L' || svg_data[i] == 'l') {
						new_rep.dimension = dimension_relative::larger;
					} else if(svg_data[i] == 'D' || svg_data[i] == 'd') {
						new_rep.dimension = dimension_relative::diagonal;
					} else if(svg_data[i] == 'P' || svg_data[i] == 'p') {
						new_rep.dimension = dimension_relative::pixel;
					}
					while(i < count) {
						if(svg_data[i] == ';')
							break;
						if(svg_data[i] == ']') {
							--i;
							break;
						}
						++i;
					}
					++stage;
				} else if(stage == 1) { // read scale
					auto start = i;
					auto end = i;
					while(i < count) {
						if(svg_data[i] == ';') {
							end = i;
							break;
						}
						if(svg_data[i] == ']') {
							end = i;
							--i;
							break;
						}
						++i;
					}
					while(start < end && svg_data[start] == ' ')
						++start;

					std::from_chars(svg_data.data() + start, svg_data.data() + end, new_rep.scale);
					++stage;
				} else if(stage == 2) { // read offset
					auto start = i;
					auto end = i;
					while(i < count) {
						if(svg_data[i] == ';') {
							end = i;
							break;
						}
						if(svg_data[i] == ']') {
							end = i;
							--i;
							break;
						}
						++i;
					}
					while(start < end && svg_data[start] == ' ')
						++start;

					std::from_chars(svg_data.data() + start, svg_data.data() + end, new_rep.offset);
					++stage;
				}

				++i;
			}
		}
	}
}

void svg::release_renders() {
	renders.clear();
}

uint32_t svg::get_render(sys::state& state, float size_x, float size_y, int32_t grid_size, float scale, float r, float g, float b) {
	uint64_t colorid = uint64_t(r * 255.0f) | (uint64_t(g * 255.0f) << uint64_t(8)) | (uint64_t(b * 255.0f) << uint64_t(16));
	uint64_t idx = uint64_t(uint32_t(size_x * grid_size)) | (uint64_t(uint32_t(size_y * grid_size)) << uint64_t(20)) | (colorid << 40);

	if(auto it = renders.find(idx); it != renders.end()) {
		return it->second.texture_handle;
	}
	return make_new_render(state, size_x, size_y, grid_size, scale);
}
uint32_t svg::try_get_render(float size_x, float size_y, int32_t grid_size, float r, float g, float b) {
	uint64_t colorid = uint64_t(r * 255.0f) | (uint64_t(g * 255.0f) << uint64_t(8)) | (uint64_t(b * 255.0f) << uint64_t(16));
	uint64_t idx = uint64_t(uint32_t(size_x * grid_size)) | (uint64_t(uint32_t(size_y * grid_size)) << uint64_t(20)) | (colorid << 40);

	if(auto it = renders.find(idx); it != renders.end()) {
		return it->second.texture_handle;
	}
	return 0;
}
uint32_t svg::make_new_render(sys::state& state, float size_x, float size_y, int32_t grid_size, float scale, float r, float g, float b) {
	if(svg_data.size() == 0)
		return 0;

	char temp_buffer[128] = { 0 };

	float x_scale = float(size_x * 500.0f) / float(base_width);
	float y_scale = float(size_y * 500.0f) / float(base_height);
	float s_scale = std::min(x_scale, y_scale);
	float l_scale = std::max(x_scale, y_scale);
	float d_scale = std::sqrt(x_scale * x_scale + y_scale * y_scale);
	float p_scale = 500.0f / float(grid_size);

	for(auto& reos : replacements) {
		float chosen_scale = x_scale;
		switch(reos.dimension) {
			case dimension_relative::height: chosen_scale = y_scale; break;
			case dimension_relative::width: chosen_scale = x_scale; break;
			case dimension_relative::smaller: chosen_scale = s_scale; break;
			case dimension_relative::larger: chosen_scale = l_scale; break;
			case dimension_relative::diagonal: chosen_scale = d_scale; break;
			case dimension_relative::pixel: chosen_scale = p_scale; break;
		}
		if(!reos.emit_quotes) {
			auto result = std::to_chars(temp_buffer, temp_buffer + 128, chosen_scale * reos.scale + reos.offset);
			memset(result.ptr, ' ', size_t((temp_buffer + 128) - result.ptr));
			memcpy(svg_data.data() + reos.start_position, temp_buffer, size_t(std::min(reos.end_position - reos.start_position, uint32_t(128))));
		} else {
			auto result = std::to_chars(temp_buffer + 1, temp_buffer + 126, chosen_scale * reos.scale + reos.offset);
			memset(result.ptr, ' ', size_t((temp_buffer + 128) - result.ptr));
			*result.ptr = '\"';
			temp_buffer[0] = '\"';
			memcpy(svg_data.data() + reos.start_position, temp_buffer, size_t(std::min(reos.end_position - reos.start_position, uint32_t(128))));
		}
	}

	char cssstylesheet[] = ".primarycolor { fill: #000000; stroke: #000000; } ";
	auto const clroffset = strlen(".primarycolor { fill: #");
	auto const clroffset2 = strlen(".primarycolor { fill: #000000; stroke: #");
	auto tohexdigit = [](uint32_t v) { 
		char table[] = "0123456789abcdef";
		return table[v & 0x0F];
	};
	auto rv = uint32_t(r * 255.0f);
	cssstylesheet[clroffset] = cssstylesheet[clroffset2] = tohexdigit(rv >> 4);
	cssstylesheet[clroffset+1] = cssstylesheet[clroffset2 + 1] = tohexdigit(rv);
	auto gv = uint32_t(g * 255.0f);
	cssstylesheet[clroffset + 2] = cssstylesheet[clroffset2 + 2] = tohexdigit(gv >> 4);
	cssstylesheet[clroffset + 3] = cssstylesheet[clroffset2 + 3] = tohexdigit(gv);
	auto bv = uint32_t(b * 255.0f);
	cssstylesheet[clroffset + 4] = cssstylesheet[clroffset2 + 4] = tohexdigit(bv >> 4);
	cssstylesheet[clroffset + 5] = cssstylesheet[clroffset2 + 5] = tohexdigit(bv);

	auto doc = lunasvg::Document::loadFromData(svg_data.data(), svg_data.size(), [&state](std::string_view file_name) {
		return state.svg_image_files.get_file_data(state, file_name);
	});

	if(!doc) std::abort(); // TODO: error message
	doc->applyStyleSheet(cssstylesheet);

	lunasvg::Bitmap bmp(
		int32_t(size_x * scale * grid_size),
		int32_t(size_y * scale * grid_size));

	doc->render(bmp, lunasvg::Matrix{ }.scale(scale * float(grid_size) / 500.0f, scale * float(grid_size) / 500.0f));

	bmp.convertToRGBA();

	svg_instance new_inst((char const*)(bmp.data()),
		int32_t(size_x * scale * grid_size),
		int32_t(size_y * scale * grid_size));

	auto h = new_inst.texture_handle;

	uint64_t colorid = uint64_t(r * 255.0f) | (uint64_t(g * 255.0f) << uint64_t(8)) | (uint64_t(b * 255.0f) << uint64_t(16));
	uint64_t idx = uint64_t(uint32_t(size_x * grid_size)) | (uint64_t(uint32_t(size_y * grid_size)) << uint64_t(20)) | (colorid << 40);
	renders[idx] = std::move(new_inst);

	return h;
}


simple_svg::simple_svg(char const* data, size_t count) : svg_data(data, data + count) {

}

void simple_svg::release_renders() {
	renders.clear();
}

uint32_t simple_svg::get_render(sys::state& state, int32_t size_x, int32_t size_y, float scale, float r, float g, float b) {
	uint64_t colorid = uint64_t(r * 255.0f) | (uint64_t(g * 255.0f) << uint64_t(8)) | (uint64_t(b * 255.0f) << uint64_t(16));
	uint64_t idx = uint64_t(uint32_t(size_x)) | (uint64_t(uint32_t(size_y)) << uint64_t(20)) | (colorid << 40);

	if(auto it = renders.find(idx); it != renders.end()) {
		return it->second.texture_handle;
	}
	return make_new_render(state, size_x, size_y, scale, r, g, b);
}
uint32_t simple_svg::try_get_render(int32_t size_x, int32_t size_y, float r, float g, float b) {
	uint64_t colorid = uint64_t(r * 255.0f) | (uint64_t(g * 255.0f) << uint64_t(8)) | (uint64_t(b * 255.0f) << uint64_t(16));
	uint64_t idx = uint64_t(uint32_t(size_x)) | (uint64_t(uint32_t(size_y)) << uint64_t(20)) | (colorid << 40);

	if(auto it = renders.find(idx); it != renders.end()) {
		return it->second.texture_handle;
	}
	return 0;
}
uint32_t simple_svg::make_new_render(sys::state& state, int32_t size_x, int32_t size_y, float scale, float r, float g, float b) {
	if(svg_data.size() == 0)
		return 0;

	char cssstylesheet[] = ".primarycolor { fill: #000000; stroke: #000000; } ";
	auto const clroffset = strlen(".primarycolor { fill: #");
	auto const clroffset2 = strlen(".primarycolor { fill: #000000; stroke: #");
	auto tohexdigit = [](uint32_t v) {
		char table[] = "0123456789abcdef";
		return table[v & 0x0F];
	};
	auto rv = uint32_t(r * 255.0f);
	cssstylesheet[clroffset] = cssstylesheet[clroffset2] = tohexdigit(rv >> 4);
	cssstylesheet[clroffset + 1] = cssstylesheet[clroffset2 + 1] = tohexdigit(rv);
	auto gv = uint32_t(g * 255.0f);
	cssstylesheet[clroffset + 2] = cssstylesheet[clroffset2 + 2] = tohexdigit(gv >> 4);
	cssstylesheet[clroffset + 3] = cssstylesheet[clroffset2 + 3] = tohexdigit(gv);
	auto bv = uint32_t(b * 255.0f);
	cssstylesheet[clroffset + 4] = cssstylesheet[clroffset2 + 4] = tohexdigit(bv >> 4);
	cssstylesheet[clroffset + 5] = cssstylesheet[clroffset2 + 5] = tohexdigit(bv);

	auto doc = lunasvg::Document::loadFromData(svg_data.data(), svg_data.size(), [&state](std::string_view file_name) {
		return state.svg_image_files.get_file_data(state, file_name);
	});

	if(!doc) std::abort(); // TODO: error message
	doc->applyStyleSheet(cssstylesheet);

	lunasvg::Bitmap bmp(
		int32_t(size_x * scale),
		int32_t(size_y * scale));

	doc->render(bmp, lunasvg::Matrix{ }.scale(scale * size_x / float(doc->width()), scale * size_y / float(doc->height())));
	bmp.convertToRGBA();

	svg_instance new_inst((char const*)(bmp.data()),
		int32_t(size_x * scale),
		int32_t(size_y * scale));

	auto h = new_inst.texture_handle;

	uint64_t colorid = uint64_t(r * 255.0f) | (uint64_t(g * 255.0f) << uint64_t(8)) | (uint64_t(b * 255.0f) << uint64_t(16));
	uint64_t idx = uint64_t(uint32_t(size_x)) | (uint64_t(uint32_t(size_y)) << uint64_t(20)) | (colorid << 40);
	renders[idx] = std::move(new_inst);

	return h;
}

std::pair<void const*, int> file_bank::get_file_data(sys::state& state, std::string_view file_name) {
	if(auto it = file_contents.find(file_name); it != file_contents.end()) {
		return std::pair<void const*, int>{(void const*)(it->second.data()), int(it->second.size()) };
	} else {
		auto root = simple_fs::get_root(state.common_fs);
		auto assets_dir = simple_fs::open_directory(root, NATIVE("assets"));
		auto svg_dir = simple_fs::open_directory(assets_dir, root_directory);
		auto file = simple_fs::open_file(svg_dir, simple_fs::utf8_to_native(file_name));
		if(file) {
			auto content = simple_fs::view_contents(*file);
			std::vector<char> hold_data(content.data, content.data + content.file_size);
			std::pair<void const*, int> result{ (void const*)(hold_data.data()), int(hold_data.size()) };
			file_contents[file_name] = std::move(hold_data);
			return result;
		} else {
			file_contents[file_name] = std::vector<char>{ };
			return std::pair<void const*, int>{nullptr, 0};
		}
	}
}

}
