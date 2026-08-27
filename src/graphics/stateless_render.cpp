#include "stateless_render.hpp"
#include "fonts.hpp"
#include "constants.hpp"

namespace ui {
ogl::color_modification get_color_modification(bool is_under_mouse, bool is_disabled, bool is_interactable) {
	if(!is_under_mouse || !is_interactable) {
		if(is_disabled) {
			return ogl::color_modification::disabled;
		} else {
			return ogl::color_modification::none;
		}
	} else {
		if(is_disabled) {
			return ogl::color_modification::interactable_disabled;
		} else {
			return ogl::color_modification::interactable;
		}
	}
}
}

namespace ogl {

GLuint map_color_modification_to_index(color_modification e) {
	switch(e) {
	case color_modification::disabled:
		return parameters::disabled;
	case color_modification::interactable:
		return parameters::interactable;
	case color_modification::interactable_disabled:
		return parameters::interactable_disabled;
	default:
	case color_modification::none:
		return parameters::enabled;
	}
}
}

namespace stateless_ogl {
void text_render(
	FT_Library lib,
	GLuint square_buffer,
	float ui_scale,
	GLuint ui_shader_subroutines_index_uniform,
	unsigned int subroutine_1,
	unsigned int subroutine_2,
	GLuint ui_shader_d_rect_uniform,
	GLuint ui_shader_subrect_uniform,
	const std::vector<text::stored_glyph>& glyph_info,
	unsigned int glyph_count,
	float x,
	float baseline_y,
	float size,
	text::font& f
) {
	glBindVertexBuffer(0, square_buffer, 0, sizeof(GLfloat) * 4);
	glUniform2ui(ui_shader_subroutines_index_uniform, subroutine_1, subroutine_2);

	auto& font_instance = f.retrieve_stateless_instance(lib, int32_t(size * ui_scale));

	x = std::floor(x * ui_scale);
	baseline_y = std::floor(baseline_y * ui_scale);

	for(unsigned int i = 0; i < glyph_count; i++) {
		hb_codepoint_t glyphid = glyph_info[i].codepoint;

		auto pixel_x_off = x + float(glyph_info[i].x_offset) / text::fixed_to_fp;
		auto trunc_pixel_x_off = std::floor(pixel_x_off);
		auto frac_pixel_off = pixel_x_off - trunc_pixel_x_off;

		int32_t subpixel = 0;
		pixel_x_off = (trunc_pixel_x_off);

		if(frac_pixel_off < 0.125f) {

		} else if(frac_pixel_off < 0.375f) {
			subpixel = 1;
		} else if(frac_pixel_off < 0.625f) {
			subpixel = 2;
		} else if(frac_pixel_off < 0.875f) {
			subpixel = 3;
		} else {
			pixel_x_off = trunc_pixel_x_off + 1.0f;
		}

		font_instance.make_glyph(uint16_t(glyphid), subpixel);
		auto& gso = font_instance.get_glyph(uint16_t(glyphid), subpixel);
		float x_advance = float(glyph_info[i].x_advance) / text::fixed_to_fp;

		if(gso.width != 0) {
			float x_offset = pixel_x_off + float(gso.bitmap_left);
			float y_offset = float(-gso.bitmap_top) - float(glyph_info[i].y_offset) / text::fixed_to_fp;

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, font_instance.textures[gso.tx_sheet]);

			glUniform4f(ui_shader_d_rect_uniform, x_offset / ui_scale, (baseline_y + y_offset) / ui_scale, float(gso.width) / ui_scale, float(gso.height) / ui_scale);
			glUniform4f(ui_shader_subrect_uniform, float(gso.x) / float(1024) /* x offset */,
					float(gso.width) / float(1024) /* x width */, float(gso.y) / float(1024) /* y offset */,
					float(gso.height) / float(1024) /* y height */
			);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		x += x_advance;
		baseline_y -= (float(glyph_info[i].y_advance) / text::fixed_to_fp);
	}
}
}
