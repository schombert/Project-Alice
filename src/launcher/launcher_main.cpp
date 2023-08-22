#ifndef UNICODE
#define UNICODE
#endif
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <Windowsx.h>
#include "Objbase.h"
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include "glew.h"
#include "wglew.h"
#include <cassert>
#include "resource.h"

#pragma comment(lib, "Ole32.lib")

#include "fonts.hpp"
#include "texture.hpp"
#include "text.hpp"
#include "simple_fs_win.cpp"
#include "prng.cpp"

namespace launcher {

float scaling_factor = 1.0f;
float dpi = 96.0f;
constexpr inline float base_width = 880.0f;
constexpr inline float base_height = 540.0f;

constexpr inline float caption_width = 837.0f;
constexpr inline float caption_height = 44.0f;

int32_t mouse_x = 0;
int32_t mouse_y = 0;

HWND m_hwnd = nullptr;

struct ui_active_rect {
	int32_t x = 0;
	int32_t y = 0;
	int32_t width = 0;
	int32_t height = 0;
};

constexpr inline int32_t ui_obj_close = 0;
constexpr inline int32_t ui_obj_list_left = 1;
constexpr inline int32_t ui_obj_list_right = 2;
constexpr inline int32_t ui_obj_create_scenario = 3;
constexpr inline int32_t ui_obj_play_game = 4;

constexpr inline int32_t ui_list_count = 14;

constexpr inline int32_t ui_list_first = 5;
constexpr inline int32_t ui_list_checkbox = 0;
constexpr inline int32_t ui_list_move_up = 1;
constexpr inline int32_t ui_list_move_down = 2;
constexpr inline int32_t ui_list_end = ui_list_first + ui_list_count * 3;

constexpr inline int32_t ui_row_height = 32;

int32_t obj_under_mouse = -1;

constexpr inline ui_active_rect ui_rects[] = {
	ui_active_rect{ 880 - 31,  0 , 31, 31}, // close
	ui_active_rect{ 26, 208, 21, 93}, // left
	ui_active_rect{ 511, 208, 21, 93}, // right
	ui_active_rect{ 555, 47, 286, 33 }, // create scenario
	ui_active_rect{ 555, 196, 286, 33 }, // play game

	ui_active_rect{ 60 + 6, 75 + 32 * 0 + 4, 24, 24 },
	ui_active_rect{ 60 + 383, 75 + 32 * 0 + 4, 24, 24 },
	ui_active_rect{ 60 + 412, 75 + 32 * 0 + 4, 24, 24 },
	ui_active_rect{ 60 + 6, 75 + 32 * 1 + 4, 24, 24 },
	ui_active_rect{ 60 + 383, 75 + 32 * 1 + 4, 24, 24 },
	ui_active_rect{ 60 + 412, 75 + 32 * 1 + 4, 24, 24 },
	ui_active_rect{ 60 + 6, 75 + 32 * 2 + 4, 24, 24 },
	ui_active_rect{ 60 + 383, 75 + 32 * 2 + 4, 24, 24 },
	ui_active_rect{ 60 + 412, 75 + 32 * 2 + 4, 24, 24 },
	ui_active_rect{ 60 + 6, 75 + 32 * 3 + 4, 24, 24 },
	ui_active_rect{ 60 + 383, 75 + 32 * 3 + 4, 24, 24 },
	ui_active_rect{ 60 + 412, 75 + 32 * 3 + 4, 24, 24 },
	ui_active_rect{ 60 + 6, 75 + 32 * 4 + 4, 24, 24 },
	ui_active_rect{ 60 + 383, 75 + 32 * 4 + 4, 24, 24 },
	ui_active_rect{ 60 + 412, 75 + 32 * 4 + 4, 24, 24 },
	ui_active_rect{ 60 + 6, 75 + 32 * 5 + 4, 24, 24 },
	ui_active_rect{ 60 + 383, 75 + 32 * 5 + 4, 24, 24 },
	ui_active_rect{ 60 + 412, 75 + 32 * 5 + 4, 24, 24 },
	ui_active_rect{ 60 + 6, 75 + 32 * 6 + 4, 24, 24 },
	ui_active_rect{ 60 + 383, 75 + 32 * 6 + 4, 24, 24 },
	ui_active_rect{ 60 + 412, 75 + 32 * 6 + 4, 24, 24 },
	ui_active_rect{ 60 + 6, 75 + 32 * 7 + 4, 24, 24 },
	ui_active_rect{ 60 + 383, 75 + 32 * 7 + 4, 24, 24 },
	ui_active_rect{ 60 + 412, 75 + 32 * 7 + 4, 24, 24 },
	ui_active_rect{ 60 + 6, 75 + 32 * 8 + 4, 24, 24 },
	ui_active_rect{ 60 + 383, 75 + 32 * 8 + 4, 24, 24 },
	ui_active_rect{ 60 + 412, 75 + 32 * 8 + 4, 24, 24 },
	ui_active_rect{ 60 + 6, 75 + 32 * 9 + 4, 24, 24 },
	ui_active_rect{ 60 + 383, 75 + 32 * 9 + 4, 24, 24 },
	ui_active_rect{ 60 + 412, 75 + 32 * 9 + 4, 24, 24 },
	ui_active_rect{ 60 + 6, 75 + 32 * 10 + 4, 24, 24 },
	ui_active_rect{ 60 + 383, 75 + 32 * 10 + 4, 24, 24 },
	ui_active_rect{ 60 + 412, 75 + 32 * 10 + 4, 24, 24 },
	ui_active_rect{ 60 + 6, 75 + 32 * 11 + 4, 24, 24 },
	ui_active_rect{ 60 + 383, 75 + 32 * 11 + 4, 24, 24 },
	ui_active_rect{ 60 + 412, 75 + 32 * 11 + 4, 24, 24 },
	ui_active_rect{ 60 + 6, 75 + 32 * 12 + 4, 24, 24 },
	ui_active_rect{ 60 + 383, 75 + 32 * 12 + 4, 24, 24 },
	ui_active_rect{ 60 + 412, 75 + 32 * 12 + 4, 24, 24 },
	ui_active_rect{ 60 + 6, 75 + 32 * 13 + 4, 24, 24 },
	ui_active_rect{ 60 + 383, 75 + 32 * 13 + 4, 24, 24 },
	ui_active_rect{ 60 + 412, 75 + 32 * 13 + 4, 24, 24 },
};

HDC opengl_window_dc = nullptr;
void* opengl_context = nullptr;

void create_opengl_context() {
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	HDC window_dc = opengl_window_dc;

	int const pixel_format = ChoosePixelFormat(window_dc, &pfd);
	SetPixelFormat(window_dc, pixel_format, &pfd);

	auto handle_to_ogl_dc = wglCreateContext(window_dc);
	wglMakeCurrent(window_dc, handle_to_ogl_dc);

	glewExperimental = GL_TRUE;

	if(glewInit() != 0) {
		MessageBoxW(m_hwnd, L"GLEW failed to initialize", L"GLEW error", MB_OK);
		std::abort();
	}

	if(!wglewIsSupported("WGL_ARB_create_context")) {
		MessageBoxW(m_hwnd, L"WGL_ARB_create_context not supported", L"OpenGL error", MB_OK);
		std::abort();
	}

	// Explicitly request for OpenGL 4.5
	static const int attribs[] = { WGL_CONTEXT_MAJOR_VERSION_ARB, 4, WGL_CONTEXT_MINOR_VERSION_ARB, 5, WGL_CONTEXT_FLAGS_ARB, 0,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB, 0 };
	opengl_context = wglCreateContextAttribsARB(window_dc, nullptr, attribs);
	if(opengl_context == nullptr) {
		MessageBoxW(m_hwnd, L"Unable to create WGL context", L"OpenGL error", MB_OK);
		std::abort();
	}

	wglMakeCurrent(window_dc, HGLRC(opengl_context));
	wglDeleteContext(handle_to_ogl_dc);
	
	if(wglewIsSupported("WGL_EXT_swap_control_tear") == 1) {
		wglSwapIntervalEXT(-1);
	} else if(wglewIsSupported("WGL_EXT_swap_control") == 1) {
		wglSwapIntervalEXT(1);
	} else {
		MessageBoxW(m_hwnd, L"WGL_EXT_swap_control_tear and WGL_EXT_swap_control not supported", L"OpenGL error", MB_OK);
	}
}

void shutdown_opengl() {
	wglMakeCurrent(opengl_window_dc, nullptr);
	wglDeleteContext(HGLRC(opengl_context));
	opengl_context = nullptr;
}

bool update_under_mouse() { // return if the selected object (if any) has changed
	for(int32_t i = 0; i < ui_list_end; ++i) {
		if(mouse_x >= ui_rects[i].x && mouse_x < ui_rects[i].x + ui_rects[i].width
			&& mouse_y >= ui_rects[i].y && mouse_y < ui_rects[i].y + ui_rects[i].height) {

			if(obj_under_mouse != i) {
				obj_under_mouse = i;
				return true;
			} else {
				return false;
			}
		}
	}

	if(obj_under_mouse != -1) {
		obj_under_mouse = -1;
		return true;
	} else {
		return false;
	}
}

void mouse_click() {
	if(obj_under_mouse == -1)
		return;

	switch(obj_under_mouse) {
		case ui_obj_close:
			PostMessageW(m_hwnd, WM_CLOSE, 0, 0);
			return;
		case ui_obj_list_left :
			InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
			return;
		case ui_obj_list_right:
			InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
			return;
		case ui_obj_create_scenario:
			InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
			return;
		case ui_obj_play_game:
			return;
		default:
			break;
	}

	int32_t list_position = (obj_under_mouse - ui_list_first) / 3;
	int32_t sub_obj = obj_under_mouse - list_position * 3;

	switch(sub_obj) {
		case ui_list_checkbox:
			InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
			return;
		case ui_list_move_up:
			InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
			return;
		case ui_list_move_down:
			InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
			return;
		default:
			break;
	}
}

GLint compile_shader(std::string_view source, GLenum type) {
	GLuint return_value = glCreateShader(type);

	if(return_value == 0) {
		MessageBoxW(m_hwnd, L"shader creation failed", L"OpenGL error", MB_OK);
	}

	std::string s_source(source);
	GLchar const* texts[] = {
		"#version 430 core\r\n",
		"#extension GL_ARB_explicit_uniform_location : enable\r\n",
		"#extension GL_ARB_explicit_attrib_location : enable\r\n",
		"#extension GL_ARB_shader_subroutine : enable\r\n",
		"#define M_PI 3.1415926535897932384626433832795\r\n",
		"#define PI 3.1415926535897932384626433832795\r\n",
		s_source.c_str()
	};
	glShaderSource(return_value, 7, texts, nullptr);
	glCompileShader(return_value);

	GLint result;
	glGetShaderiv(return_value, GL_COMPILE_STATUS, &result);
	if(result == GL_FALSE) {
		GLint log_length = 0;
		glGetShaderiv(return_value, GL_INFO_LOG_LENGTH, &log_length);

		auto log = std::unique_ptr<char[]>(new char[static_cast<size_t>(log_length)]);
		GLsizei written = 0;
		glGetShaderInfoLog(return_value, log_length, &written, log.get());
		auto error = std::string("Shader failed to compile:\n") + log.get();
		MessageBoxA(m_hwnd, error.c_str(), "OpenGL error", MB_OK);
	}
	return return_value;
}

GLuint create_program(std::string_view vertex_shader, std::string_view fragment_shader) {
	GLuint return_value = glCreateProgram();
	if(return_value == 0) {
		MessageBoxW(m_hwnd, L"program creation failed", L"OpenGL error", MB_OK);
	}

	auto v_shader = compile_shader(vertex_shader, GL_VERTEX_SHADER);
	auto f_shader = compile_shader(fragment_shader, GL_FRAGMENT_SHADER);

	glAttachShader(return_value, v_shader);
	glAttachShader(return_value, f_shader);
	glLinkProgram(return_value);

	GLint result;
	glGetProgramiv(return_value, GL_LINK_STATUS, &result);
	if(result == GL_FALSE) {
		GLint logLen;
		glGetProgramiv(return_value, GL_INFO_LOG_LENGTH, &logLen);

		char* log = new char[static_cast<size_t>(logLen)];
		GLsizei written;
		glGetProgramInfoLog(return_value, logLen, &written, log);
		auto err = std::string("Program failed to link:\n") + log;
		MessageBoxA(m_hwnd, err.c_str(), "OpenGL error", MB_OK);
	}

	glDeleteShader(v_shader);
	glDeleteShader(f_shader);

	return return_value;
}

static GLfloat global_square_data[] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f };
static GLfloat global_square_right_data[] = { 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f };
static GLfloat global_square_left_data[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f };
static GLfloat global_square_flipped_data[] = { 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f };
static GLfloat global_square_right_flipped_data[] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f };
static GLfloat global_square_left_flipped_data[] = { 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };

GLuint ui_shader_program = 0;

void load_shaders() {
	simple_fs::file_system fs;
	simple_fs::add_root(fs, L".");
	auto root = get_root(fs);

	auto ui_fshader = open_file(root, NATIVE("assets/shaders/ui_f_shader.glsl"));
	auto ui_vshader = open_file(root, NATIVE("assets/shaders/ui_v_shader.glsl"));
	if(bool(ui_fshader) && bool(ui_vshader)) {
		auto vertex_content = view_contents(*ui_vshader);
		auto fragment_content = view_contents(*ui_fshader);
		ui_shader_program = create_program(std::string_view(vertex_content.data, vertex_content.file_size), std::string_view(fragment_content.data, fragment_content.file_size));
	} else {
		MessageBoxW(m_hwnd, L"Unable to open a necessary shader file", L"OpenGL error", MB_OK);
	}
}

GLuint global_square_vao = 0;
GLuint global_square_buffer = 0;
GLuint global_square_right_buffer = 0;
GLuint global_square_left_buffer = 0;
GLuint global_square_flipped_buffer = 0;
GLuint global_square_right_flipped_buffer = 0;
GLuint global_square_left_flipped_buffer = 0;

GLuint sub_square_buffers[64] = { 0 };

void load_global_squares() {
	glGenBuffers(1, &global_square_buffer);

	// Populate the position buffer
	glBindBuffer(GL_ARRAY_BUFFER, global_square_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_square_data, GL_STATIC_DRAW);

	glGenVertexArrays(1, &global_square_vao);
	glBindVertexArray(global_square_vao);
	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // texture coordinates

	glBindVertexBuffer(0, global_square_buffer, 0, sizeof(GLfloat) * 4);

	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, 0);					 // position
	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2);	// texture coordinates
	glVertexAttribBinding(0, 0);											// position -> to array zero
	glVertexAttribBinding(1, 0);											 // texture coordinates -> to array zero

	glGenBuffers(1, &global_square_left_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, global_square_left_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_square_left_data, GL_STATIC_DRAW);

	glGenBuffers(1, &global_square_right_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, global_square_right_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_square_right_data, GL_STATIC_DRAW);

	glGenBuffers(1, &global_square_right_flipped_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, global_square_right_flipped_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_square_right_flipped_data, GL_STATIC_DRAW);

	glGenBuffers(1, &global_square_left_flipped_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, global_square_left_flipped_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_square_left_flipped_data, GL_STATIC_DRAW);

	glGenBuffers(1, &global_square_flipped_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, global_square_flipped_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_square_flipped_data, GL_STATIC_DRAW);

	glGenBuffers(64, sub_square_buffers);
	for(uint32_t i = 0; i < 64; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, sub_square_buffers[i]);

		float const cell_x = static_cast<float>(i & 7) / 8.0f;
		float const cell_y = static_cast<float>((i >> 3) & 7) / 8.0f;

		GLfloat global_sub_square_data[] = { 0.0f, 0.0f, cell_x, cell_y, 0.0f, 1.0f, cell_x, cell_y + 1.0f / 8.0f, 1.0f, 1.0f,
				cell_x + 1.0f / 8.0f, cell_y + 1.0f / 8.0f, 1.0f, 0.0f, cell_x + 1.0f / 8.0f, cell_y };

		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_sub_square_data, GL_STATIC_DRAW);
	}
}


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
} // namespace parameters

enum class color_modification {
	none, disabled, interactable, interactable_disabled
};

struct color3f {
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
};

inline auto map_color_modification_to_index(color_modification e) {
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

void bind_vertices_by_rotation(ui::rotation r, bool flipped) {
	switch(r) {
		case ui::rotation::upright:
			if(!flipped)
				glBindVertexBuffer(0, global_square_buffer, 0, sizeof(GLfloat) * 4);
			else
				glBindVertexBuffer(0, global_square_flipped_buffer, 0, sizeof(GLfloat) * 4);
			break;
		case ui::rotation::r90_left:
			if(!flipped)
				glBindVertexBuffer(0, global_square_left_buffer, 0, sizeof(GLfloat) * 4);
			else
				glBindVertexBuffer(0, global_square_left_flipped_buffer, 0, sizeof(GLfloat) * 4);
			break;
		case ui::rotation::r90_right:
			if(!flipped)
				glBindVertexBuffer(0, global_square_right_buffer, 0, sizeof(GLfloat) * 4);
			else
				glBindVertexBuffer(0, global_square_right_flipped_buffer, 0, sizeof(GLfloat) * 4);
			break;
	}
}

void render_textured_rect(color_modification enabled, float x, float y, float width, float height, GLuint texture_handle, ui::rotation r, bool flipped) {
	glBindVertexArray(global_square_vao);

	bind_vertices_by_rotation(r, flipped);

	glUniform4f(parameters::drawing_rectangle, x, y, width, height);
	// glUniform4f(parameters::drawing_rectangle, 0, 0, width, height);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_handle);

	GLuint subroutines[2] = { map_color_modification_to_index(enabled), parameters::no_filter };
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_character(char codepoint, color_modification enabled, float x, float y, float size, ::text::font& f) {
	if(::text::win1250toUTF16(codepoint) != ' ') {
		// f.make_glyph(codepoint);

		glBindVertexBuffer(0, sub_square_buffers[uint8_t(codepoint) & 63], 0, sizeof(GLfloat) * 4);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, f.textures[uint8_t(codepoint) >> 6]);

		glUniform4f(parameters::drawing_rectangle, x, y, size, size);
		glUniform3f(parameters::inner_color, 0.0f, 0.0f, 0.0f);
		glUniform1f(parameters::border_size, 0.08f * 16.0f / size);

		GLuint subroutines[2] = { map_color_modification_to_index(enabled), parameters::border_filter };
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}
}

void internal_text_render(char const* codepoints, uint32_t count, float x, float baseline_y, float size,
		::text::font& f) {
	for(uint32_t i = 0; i < count; ++i) {
		if(::text::win1250toUTF16(codepoints[i]) != ' ') {
			glBindVertexBuffer(0, sub_square_buffers[uint8_t(codepoints[i]) & 63], 0, sizeof(GLfloat) * 4);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, f.textures[uint8_t(codepoints[i]) >> 6]);
			glUniform4f(parameters::drawing_rectangle, x + f.glyph_positions[uint8_t(codepoints[i])].x * size / 64.0f,
						baseline_y + f.glyph_positions[uint8_t(codepoints[i])].y * size / 64.0f, size, size);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			x += f.glyph_advances[uint8_t(codepoints[i])] * size / 64.0f +
				((i != count - 1) ? f.kerning(codepoints[i], codepoints[i + 1]) * size / 64.0f : 0.0f);

		} else {
			x += f.glyph_advances[uint8_t(codepoints[i])] * size / 64.0f +
				((i != count - 1) ? f.kerning(codepoints[i], codepoints[i + 1]) * size / 64.0f : 0.0f);
		}
	}
}

void render_new_text(char const* codepoints, uint32_t count, color_modification enabled, float x, float y, float size, color3f const& c, ::text::font& f) {
	glUniform3f(parameters::inner_color, c.r, c.g, c.b);
	glUniform1f(parameters::border_size, 0.08f * 16.0f / size);

	GLuint subroutines[2] = { map_color_modification_to_index(enabled), parameters::filter };
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines);
	internal_text_render(codepoints, count, x, y + size, size, f);
}

} // launcher::ogl

::text::font_manager font_collection;

::ogl::texture bg_tex;
::ogl::texture left_tex;
::ogl::texture right_tex;
::ogl::texture close_tex;
::ogl::texture big_button_tex;
::ogl::texture empty_check_tex;
::ogl::texture check_tex;
::ogl::texture up_tex;
::ogl::texture down_tex;

void render() {
	if(!opengl_context)
		return;

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glUseProgram(ui_shader_program);
	glUniform1f(ogl::parameters::screen_width, float(base_width));
	glUniform1f(ogl::parameters::screen_height, float(base_height));
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0, 0, base_width * scaling_factor, base_height * scaling_factor);
	glDepthRange(-1.0f, 1.0f);

	launcher::ogl::render_textured_rect(launcher::ogl::color_modification::none, 0.0f, 0.0f, base_width, base_height, bg_tex.get_texture_handle(), ui::rotation::upright, false);

	launcher::ogl::render_new_text("Project Alice", 13, launcher::ogl::color_modification::none, 78.0f, 5.0f, 26.0f, launcher::ogl::color3f{255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f}, font_collection.fonts[1]);

	launcher::ogl::render_textured_rect(obj_under_mouse == ui_obj_close ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
		ui_rects[ui_obj_close].x,
		ui_rects[ui_obj_close].y,
		ui_rects[ui_obj_close].width,
		ui_rects[ui_obj_close].height,
		close_tex.get_texture_handle(), ui::rotation::upright, false);

	launcher::ogl::render_textured_rect(obj_under_mouse == ui_obj_list_left ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
		ui_rects[ui_obj_list_left].x,
		ui_rects[ui_obj_list_left].y,
		ui_rects[ui_obj_list_left].width,
		ui_rects[ui_obj_list_left].height,
		left_tex.get_texture_handle(), ui::rotation::upright, false);

	launcher::ogl::render_textured_rect(obj_under_mouse == ui_obj_list_right ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
		ui_rects[ui_obj_list_right].x,
		ui_rects[ui_obj_list_right].y,
		ui_rects[ui_obj_list_right].width,
		ui_rects[ui_obj_list_right].height,
		right_tex.get_texture_handle(), ui::rotation::upright, false);

	launcher::ogl::render_textured_rect(obj_under_mouse == ui_obj_create_scenario ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
		ui_rects[ui_obj_create_scenario].x,
		ui_rects[ui_obj_create_scenario].y,
		ui_rects[ui_obj_create_scenario].width,
		ui_rects[ui_obj_create_scenario].height,
		big_button_tex.get_texture_handle(), ui::rotation::upright, false);

	launcher::ogl::render_textured_rect(obj_under_mouse == ui_obj_play_game ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
		ui_rects[ui_obj_play_game].x,
		ui_rects[ui_obj_play_game].y,
		ui_rects[ui_obj_play_game].width,
		ui_rects[ui_obj_play_game].height,
		big_button_tex.get_texture_handle(), ui::rotation::upright, false);

	launcher::ogl::render_new_text("Mod List", 8, launcher::ogl::color_modification::none, 306.0f, 45.0f, 24.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, font_collection.fonts[1]);

	launcher::ogl::render_new_text("Create Scenario", 15, launcher::ogl::color_modification::none, 623.0f, 50.0f, 22.0f, launcher::ogl::color3f{ 50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f }, font_collection.fonts[1]);

	launcher::ogl::render_new_text("Start Game", 10, launcher::ogl::color_modification::none, 642.0f, 199.0f, 22.0f, launcher::ogl::color3f{ 50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f }, font_collection.fonts[1]);

	for(int32_t i = 0; i < ui_list_count; ++i) {

		launcher::ogl::render_textured_rect(obj_under_mouse == ui_list_first + 3 * i + ui_list_checkbox ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
			ui_rects[ui_list_first + 3 * i + ui_list_checkbox].x,
			ui_rects[ui_list_first + 3 * i + ui_list_checkbox].y,
			ui_rects[ui_list_first + 3 * i + ui_list_checkbox].width,
			ui_rects[ui_list_first + 3 * i + ui_list_checkbox].height,
			check_tex.get_texture_handle(), ui::rotation::upright, false);

		launcher::ogl::render_textured_rect(obj_under_mouse == ui_list_first + 3 * i + ui_list_move_up ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
			ui_rects[ui_list_first + 3 * i + ui_list_move_up].x,
			ui_rects[ui_list_first + 3 * i + ui_list_move_up].y,
			ui_rects[ui_list_first + 3 * i + ui_list_move_up].width,
			ui_rects[ui_list_first + 3 * i + ui_list_move_up].height,
			up_tex.get_texture_handle(), ui::rotation::upright, false);

		launcher::ogl::render_textured_rect(obj_under_mouse == ui_list_first + 3 * i + ui_list_move_down ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
			ui_rects[ui_list_first + 3 * i + ui_list_move_down].x,
			ui_rects[ui_list_first + 3 * i + ui_list_move_down].y,
			ui_rects[ui_list_first + 3 * i + ui_list_move_down].width,
			ui_rects[ui_list_first + 3 * i + ui_list_move_down].height,
			down_tex.get_texture_handle(), ui::rotation::upright, false);

		launcher::ogl::render_new_text("Mod Name", 8, launcher::ogl::color_modification::none, 250.0f, 75.0f + 4.0f + i * ui_row_height, 20.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, font_collection.fonts[0]);
	}

	SwapBuffers(opengl_window_dc);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if(message == WM_CREATE) {
		opengl_window_dc = GetDC(hwnd);

		create_opengl_context();

		glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
		glEnable(GL_LINE_SMOOTH);

		load_shaders();				// create shaders
		load_global_squares();		// create various squares to drive the shaders with

		simple_fs::file_system fs;
		simple_fs::add_root(fs, L".");
		auto root = get_root(fs);

		auto font_a = open_file(root, NATIVE("assets/fonts/LibreCaslonText-Regular.ttf"));
		if(font_a) {
			auto file_content = view_contents(*font_a);
			font_collection.load_font(font_collection.fonts[0], file_content.data, file_content.file_size);
		}
		auto font_b = open_file(root, NATIVE("assets/fonts/LibreCaslonText-Italic.ttf"));
		if(font_b) {
			auto file_content = view_contents(*font_b);
			font_collection.load_font(font_collection.fonts[1], file_content.data, file_content.file_size);
		}

		font_collection.load_all_glyphs();

		::ogl::load_file_and_return_handle(L"assets/launcher_bg.png", fs, bg_tex, false);
		::ogl::load_file_and_return_handle(L"assets/launcher_left.png", fs, left_tex, false);
		::ogl::load_file_and_return_handle(L"assets/launcher_right.png", fs, right_tex, false);
		::ogl::load_file_and_return_handle(L"assets/launcher_close.png", fs, close_tex, false);
		::ogl::load_file_and_return_handle(L"assets/launcher_big_button.png", fs, big_button_tex, false);
		::ogl::load_file_and_return_handle(L"assets/launcher_no_check.png", fs, empty_check_tex, false);
		::ogl::load_file_and_return_handle(L"assets/launcher_check.png", fs, check_tex, false);
		::ogl::load_file_and_return_handle(L"assets/launcher_up.png", fs, up_tex, false);
		::ogl::load_file_and_return_handle(L"assets/launcher_down.png", fs, down_tex, false);

		return 1;
	} else {
		switch(message) {
			case WM_DPICHANGED:
			{
				dpi = float(HIWORD(wParam));
				auto lprcNewScale = reinterpret_cast<RECT*>(lParam);


				scaling_factor = float(lprcNewScale->right - lprcNewScale->left) / base_width;

				SetWindowPos(hwnd, nullptr, lprcNewScale->left, lprcNewScale->top,
					lprcNewScale->right - lprcNewScale->left, lprcNewScale->bottom - lprcNewScale->top,
					SWP_NOZORDER | SWP_NOACTIVATE);

				InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);

				break;
			}
			case WM_NCMOUSEMOVE:
			{
				RECT rcWindow;
				GetWindowRect(hwnd, &rcWindow);
				auto x = GET_X_LPARAM(lParam);
				auto y = GET_Y_LPARAM(lParam);

				POINTS adj{ SHORT(x - rcWindow.left), SHORT(y - rcWindow.top) };
				memcpy(&lParam, &adj, sizeof(LPARAM));
			} // fallthrough; keep above WM_MOUSEMOVE
			case WM_MOUSEMOVE:
			{
				mouse_x = int32_t(float(GET_X_LPARAM(lParam)) / scaling_factor);
				mouse_y = int32_t(float(GET_Y_LPARAM(lParam)) / scaling_factor);
				if(update_under_mouse()) {
					InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
				}
				return 0;
			}
			case WM_LBUTTONDOWN:
			{
				mouse_click();
				return 0;
			}
			case WM_NCCALCSIZE:
				if(wParam == TRUE)
					return 0;
				break;
			case WM_NCHITTEST:
			{
				POINT ptMouse = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				RECT rcWindow;
				GetWindowRect(hwnd, &rcWindow);

				if(ptMouse.x <= int32_t(rcWindow.left + caption_width * scaling_factor)
					&& ptMouse.y <= int32_t(rcWindow.top + caption_height * scaling_factor)) {

					return HTCAPTION;
				} else {
					return HTCLIENT;
				}
			}
			case WM_PAINT:
			case WM_DISPLAYCHANGE:
			{
				PAINTSTRUCT ps;
				BeginPaint(hwnd, &ps);

				render();

				EndPaint(hwnd, &ps);
				return 0;
			}
			case WM_DESTROY:
				PostQuitMessage(0);
				return 1;
			default:
				break;

		}
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
}

} // end launcher namespace

int WINAPI wWinMain(
	HINSTANCE /*hInstance*/,
	HINSTANCE /*hPrevInstance*/,
	LPWSTR /*lpCmdLine*/,
	int /*nCmdShow*/
) {
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	if(!SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
		return 0;

	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
	wcex.style = CS_OWNDC;
	wcex.lpfnWndProc = launcher::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR);
	wcex.hInstance = GetModuleHandle(nullptr);
	wcex.hIcon = (HICON)LoadImage(GetModuleHandleW(nullptr), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), 0);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.lpszClassName = L"alice_launcher_class";

	if(RegisterClassEx(&wcex) == 0) {
		std::abort();
	}

	launcher::m_hwnd = CreateWindowEx(
		0,
		L"alice_launcher_class",
		L"Launch Project Alice",
		WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_BORDER,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		0,
		0,
		NULL,
		NULL,
		GetModuleHandle(nullptr),
		nullptr
	);

	if(launcher::m_hwnd) {

		launcher::dpi = float(GetDpiForWindow((HWND)(launcher::m_hwnd)));

		auto monitor_handle = MonitorFromWindow((HWND)(launcher::m_hwnd), MONITOR_DEFAULTTOPRIMARY);
		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		GetMonitorInfo(monitor_handle, &mi);

		auto vert_space = mi.rcWork.bottom - mi.rcWork.top;
		float rough_scale = float(vert_space) / 1080.0f;
		if(rough_scale >= 1.0f) {
			launcher::scaling_factor = std::round(rough_scale);
		} else {
			launcher::scaling_factor = std::round(rough_scale * 4.0f) / 4.0f;
		}

		SetWindowPos(
			(HWND)(launcher::m_hwnd),
			NULL,
			NULL,
			NULL,
			static_cast<int>(launcher::scaling_factor * launcher::base_width),
			static_cast<int>(launcher::scaling_factor * launcher::base_height),
			SWP_NOMOVE | SWP_FRAMECHANGED);


		ShowWindow((HWND)(launcher::m_hwnd), SW_SHOWNORMAL);
		UpdateWindow((HWND)(launcher::m_hwnd));
	}

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0)) {
		DispatchMessage(&msg);
	}

	CoUninitialize();

	return 0;
}

#include "fonts.cpp"
#include "texture.cpp"
#include "text.cpp"
#include "system_state.cpp"
#include "parsers.cpp"
#include "defines.cpp"
#include "float_from_chars.cpp"
#include "gui_graphics_parsers.cpp"
#include "nations_parsing.cpp"
#include "cultures_parsing.cpp"
#include "econ_parsing.cpp"
#include "military_parsing.cpp"
#include "date_interface.cpp"
#include "provinces_parsing.cpp"
#include "trigger_parsing.cpp"
#include "effect_parsing.cpp"
#include "serialization.cpp"
#include "nations.cpp"
#include "culture.cpp"
#include "military.cpp"
#include "modifiers.cpp"
#include "province.cpp"
#include "triggers.cpp"
#include "effects.cpp"
#include "economy.cpp"
#include "demographics.cpp"
#include "bmfont.cpp"
#include "rebels.cpp"
#include "parsers_declarations.cpp"
#include "politics.cpp"
#include "events.cpp"
#include "gui_graphics.cpp"
#include "gui_element_types.cpp"
#include "gui_common_elements.cpp"
#include "gui_main_menu.cpp"
#include "gui_console.cpp"
#include "gui_province_window.cpp"
#include "gui_budget_window.cpp"
#include "gui_technology_window.cpp"
#include "gui_event.cpp"
#include "gui_message_settings_window.cpp"
#include "gui_trigger_tooltips.cpp"
#include "gui_effect_tooltips.cpp"
#include "gui_modifier_tooltips.cpp"
#include "commands.cpp"
#include "diplomatic_messages.cpp"
#include "notifications.cpp"
#include "map_tooltip.cpp"
#include "unit_tooltip.cpp"
#include "ai.cpp"

#include "window_win.cpp"
#include "sound_win.cpp"
#include "opengl_wrapper_win.cpp"
#include "opengl_wrapper.cpp"

namespace sys {
state::~state() {
	// why does this exist ? So that the destructor of the unique pointer doesn't have to be known before it is implemented
}
} // namespace sys

// zstd

#define XXH_NAMESPACE ZSTD_
#define ZSTD_DISABLE_ASM

#include "zstd/xxhash.c"
#include "zstd/zstd_decompress_block.c"
#include "zstd/zstd_ddict.c"
#include "zstd/huf_compress.c"
#include "zstd/fse_compress.c"
#include "zstd/huf_decompress.c"
#include "zstd/fse_decompress.c"
#include "zstd/zstd_common.c"
#include "zstd/entropy_common.c"
#include "zstd/hist.c"
#include "zstd/zstd_compress_superblock.c"
#include "zstd/zstd_ldm.c"
#include "zstd/zstd_opt.c"
#include "zstd/zstd_lazy.c"
#include "zstd/zstd_double_fast.c"
#include "zstd/zstd_fast.c"
#include "zstd/zstd_compress_literals.c"
#include "zstd/zstd_compress_sequences.c"
#include "zstd/error_private.c"
#include "zstd/zstd_decompress.c"
#include "zstd/zstd_compress.c"

