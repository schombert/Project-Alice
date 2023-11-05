#pragma once

#include <string>
#include <string_view>

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include "glew.h"

#include "container_types.hpp"
#include "texture.hpp"
#include "fonts.hpp"

namespace ogl {
namespace parameters {

inline constexpr GLuint screen_width = 0;
inline constexpr GLuint screen_height = 1;
inline constexpr GLuint drawing_rectangle = 2;

inline constexpr GLuint border_size = 6;
inline constexpr GLuint inner_color = 7;
inline constexpr GLuint subrect = 10;

inline constexpr GLuint enabled = 4;
inline constexpr GLuint disabled = 3;
inline constexpr GLuint border_filter = 0;
inline constexpr GLuint filter = 1;
inline constexpr GLuint no_filter = 2;
inline constexpr GLuint sub_sprite = 5;
inline constexpr GLuint use_mask = 6;
inline constexpr GLuint progress_bar = 7;
inline constexpr GLuint frame_stretch = 8;
inline constexpr GLuint piechart = 9;
inline constexpr GLuint barchart = 10;
inline constexpr GLuint linegraph = 11;
inline constexpr GLuint tint = 12;
inline constexpr GLuint interactable = 13;
inline constexpr GLuint interactable_disabled = 14;
inline constexpr GLuint subsprite_b = 15;
inline constexpr GLuint alternate_tint = 16;
} // namespace parameters

enum class color_modification { none, disabled, interactable, interactable_disabled };

struct color3f {
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
};

#ifndef NDEBUG
inline void debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
		GLsizei, // length
		GLchar const* message, void const*) {

	std::string source_str;
	switch(source) {
	case GL_DEBUG_SOURCE_API:
		source_str = "OpenGL API call";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		source_str = "Window system API";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		source_str = "Shading language compiler";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		source_str = "Application associated with OpenGL";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		source_str = "User generated";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		source_str = "Unknown source";
		break;
	}
	std::string error_type;
	switch(type) {
	case GL_DEBUG_TYPE_ERROR:
		error_type = "General error";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		error_type = "Deprecated behavior";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		error_type = "Undefined behavior";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		error_type = "Portability issue";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		error_type = "Performance issue";
		break;
	case GL_DEBUG_TYPE_MARKER:
		error_type = "Command stream annotation";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		error_type = "Error group push";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		error_type = "Error group pop";
		break;
	case GL_DEBUG_TYPE_OTHER:
		error_type = "Uknown error type";
		break;
	}
	std::string severity_str;
	switch(severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		severity_str = "High";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		severity_str = "Medium";
		break;
	case GL_DEBUG_SEVERITY_LOW:
		severity_str = "Low";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		severity_str = "Notification";
		break;
	}
	std::string full_message("OpenGL error ");
	full_message += std::to_string(id);
	full_message += " ";
	full_message += " source: ";
	full_message += source_str;
	full_message += " type: ";
	full_message += error_type;
	full_message += " severity: ";
	full_message += severity_str;
	full_message += "; ";
	full_message += message;
	full_message += "\n";

#ifdef _WIN64
	OutputDebugStringA(full_message.c_str());
#else
	printf("%s", full_message.c_str());
#endif
}
#endif

struct data {
	tagged_vector<texture, dcon::texture_id> asset_textures;

	void* context = nullptr;
	GLuint ui_shader_program = 0;

	GLuint global_square_vao = 0;
	GLuint global_square_buffer = 0;
	GLuint global_square_right_buffer = 0;
	GLuint global_square_left_buffer = 0;
	GLuint global_square_flipped_buffer = 0;
	GLuint global_square_right_flipped_buffer = 0;
	GLuint global_square_left_flipped_buffer = 0;

	GLuint sub_square_buffers[64] = {0};

	GLuint money_icon_tex = 0;
	GLuint cross_icon_tex = 0;
	GLuint checkmark_icon_tex = 0;
	GLuint navy_icon_tex = 0;
	GLuint army_icon_tex = 0;

	GLuint msaa_framebuffer = 0;
	GLuint msaa_texcolorbuffer = 0;
	GLuint msaa_rbo = 0;
	GLuint msaa_interbuffer = 0;
	GLuint msaa_texture = 0;
	GLuint msaa_vao = 0;
	GLuint msaa_vbo = 0;
	GLuint msaa_shader_program = 0;
	bool msaa_enabled = false;
};

void notify_user_of_fatal_opengl_error(std::string message); // this function calls std::abort

void create_opengl_context(sys::state& state); // you shouldn't call this directly; only initialize_opengl should call it
void initialize_opengl(sys::state& state);
void shutdown_opengl(sys::state& state);

bool display_tag_is_valid(sys::state& state, char tag[3]);

GLint compile_shader(std::string_view source, GLenum type);
GLuint create_program(std::string_view vertex_shader, std::string_view fragment_shader);
void load_shaders(sys::state& state);
void load_global_squares(sys::state& state);

class lines {
private:
	float* buffer = nullptr;
	GLuint buffer_handle = 0;
	bool pending_data_update = true;

public:
	uint32_t count = 0;

	lines(uint32_t c) : count(c) {
		buffer = new float[count * 4];
		set_default_y();
	}
	lines(lines&& o) noexcept
			: buffer(o.buffer), buffer_handle(o.buffer_handle), pending_data_update(o.pending_data_update), count(o.count) {
		o.buffer = nullptr;
	}
	lines& operator=(lines&& o) noexcept {
		count = o.count;
		buffer = o.buffer;
		buffer_handle = o.buffer_handle;
		pending_data_update = o.pending_data_update;

		o.buffer = nullptr;
		return *this;
	}
	~lines() {
		delete[] buffer;
	}
	void set_default_y();
	void set_y(float* v);
	void bind_buffer();
};

void render_textured_rect(sys::state const& state, color_modification enabled, float x, float y, float width, float height,
		GLuint texture_handle, ui::rotation r, bool flipped);
void render_textured_rect_direct(sys::state const& state, float x, float y, float width, float height, uint32_t handle);
void render_linegraph(sys::state const& state, color_modification enabled, float x, float y, float width, float height, lines& l);
void render_barchart(sys::state const& state, color_modification enabled, float x, float y, float width, float height,
		data_texture& t, ui::rotation r, bool flipped);
void render_piechart(sys::state const& state, color_modification enabled, float x, float y, float size, data_texture& t);
void render_bordered_rect(sys::state const& state, color_modification enabled, float border_size, float x, float y, float width,
		float height, GLuint texture_handle, ui::rotation r, bool flipped);
void render_masked_rect(sys::state const& state, color_modification enabled, float x, float y, float width, float height,
		GLuint texture_handle, GLuint mask_texture_handle, ui::rotation r, bool flipped);
void render_progress_bar(sys::state const& state, color_modification enabled, float progress, float x, float y, float width,
		float height, GLuint left_texture_handle, GLuint right_texture_handle, ui::rotation r, bool flipped);
void render_tinted_textured_rect(sys::state const& state, float x, float y, float width, float height, float r, float g, float b,
		GLuint texture_handle, ui::rotation rot, bool flipped);
void render_subsprite(sys::state const& state, color_modification enabled, int frame, int total_frames, float x, float y,
		float width, float height, GLuint texture_handle, ui::rotation r, bool flipped);
void render_tinted_subsprite(sys::state const& state, int frame, int total_frames, float x, float y,
		float width, float height, float r, float g, float b, GLuint texture_handle, ui::rotation rot, bool flipped);
void render_new_text(sys::state const& state, char const* codepoints, uint32_t count, color_modification enabled, float x,
		float y, float size, color3f const& c, text::font& f);
void render_text(sys::state& state, char const* codepoints, uint32_t count, color_modification enabled, float x, float y,
		color3f const& c, uint16_t font_id);
void render_character(sys::state const& state, char codepoint, color_modification enabled, float x, float y, float size, text::font& f);

bool msaa_enabled(sys::state const& state);
void initialize_msaa(sys::state& state, int32_t x, int32_t y);
void deinitialize_msaa(sys::state& state);

} // namespace ogl
