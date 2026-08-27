#pragma once

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include "glew.h"

#include "stateless_render_constants.hpp"
#include "fonts.hpp"

namespace text {
class font;
struct stored_glyph;
}
namespace ogl {
GLuint map_color_modification_to_index(ogl::color_modification e);
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
);
}
