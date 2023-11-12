#ifndef UNICODE
#define UNICODE
#endif

#include <Windowsx.h>
#include <shellapi.h>
#include <shellscalingapi.h>
#include "Objbase.h"
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include "glew.h"
#include "wglew.h"
#include <cassert>
#include "resource.h"

#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "Shell32.lib")

#include "local_user_settings.hpp"
#include "fonts.hpp"
#include "texture.hpp"
#include "text.hpp"
#include "simple_fs_win.cpp"
#include "prng.cpp"
extern "C" {
#include "blake2.c"
};
#include "serialization.hpp"
#include "network.cpp"

namespace launcher {

static float scaling_factor = 1.0f;
static float dpi = 96.0f;
constexpr inline float base_width = 880.0f;
constexpr inline float base_height = 540.0f;

constexpr inline float caption_width = 837.0f;
constexpr inline float caption_height = 44.0f;

static int32_t mouse_x = 0;
static int32_t mouse_y = 0;

static std::string ip_addr = "127.0.0.1";
static std::string player_name = "AnonAnon";

static HWND m_hwnd = nullptr;

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
constexpr inline int32_t ui_obj_host_game = 5;
constexpr inline int32_t ui_obj_join_game = 6;
constexpr inline int32_t ui_obj_ip_addr = 7;
constexpr inline int32_t ui_obj_player_name = 8;

constexpr inline int32_t ui_list_count = 14;

constexpr inline int32_t ui_list_first = 9;
constexpr inline int32_t ui_list_checkbox = 0;
constexpr inline int32_t ui_list_move_up = 1;
constexpr inline int32_t ui_list_move_down = 2;
constexpr inline int32_t ui_list_end = ui_list_first + ui_list_count * 3;

constexpr inline int32_t ui_row_height = 32;

constexpr inline float list_text_right_align = 420.0f;

static int32_t obj_under_mouse = -1;

constexpr inline ui_active_rect ui_rects[] = {
	ui_active_rect{ 880 - 31,  0 , 31, 31}, // close
	ui_active_rect{ 30, 208, 21, 93}, // left
	ui_active_rect{ 515, 208, 21, 93}, // right
	ui_active_rect{ 555, 48, 286, 33 }, // create scenario
	ui_active_rect{ 555, 48 + 156 * 1, 286, 33 }, // play game
	ui_active_rect{ 555, 48 + 156 * 2 + 36 * 0, 138, 33 }, // host game
	ui_active_rect{ 695, 48 + 156 * 2 + 36 * 0, 138, 33 }, // join game
	ui_active_rect{ 555, 48 + 156 * 2 + 36 * 2 - 4, 200, 33 }, // ip address textbox
	ui_active_rect{ 757, 48 + 156 * 2 + 36 * 2 - 4, 76, 33 }, // player name textbox

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

static std::vector<parsers::mod_file> mod_list;

struct scenario_file {
	native_string file_name;
	sys::mod_identifier ident;
};

static std::vector<scenario_file> scenario_files;
static native_string selected_scenario_file;
static uint32_t max_scenario_count = 0;
static std::atomic<bool> file_is_ready = true;

static int32_t frame_in_list = 0;

static HDC opengl_window_dc = nullptr;
static void* opengl_context = nullptr;

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

void recursively_remove_from_list(parsers::mod_file& mod) {
	for(int32_t i = 0; i < int32_t(mod_list.size()); ++i) {
		if(mod_list[i].mod_selected) {
			if(std::find(mod_list[i].dependent_mods.begin(), mod_list[i].dependent_mods.end(), mod.name_) != mod_list[i].dependent_mods.end()) {
				mod_list[i].mod_selected = false;
				recursively_remove_from_list(mod_list[i]);
			}
		}
	}
}
void recursively_add_to_list(parsers::mod_file& mod) {
	for(auto& dep : mod.dependent_mods) {
		for(int32_t i = 0; i < int32_t(mod_list.size()); ++i) {
			if(!mod_list[i].mod_selected && mod_list[i].name_ == dep) {
				mod_list[i].mod_selected = true;
				recursively_add_to_list(mod_list[i]);
			}
		}
	}
}

bool transitively_depends_on_internal(parsers::mod_file const& moda, parsers::mod_file const& modb, std::vector<bool>& seen_indices) {
	for(auto& dep : moda.dependent_mods) {
		if(dep == modb.name_)
			return true;

		for(int32_t i = 0; i < int32_t(mod_list.size()); ++i) {
			if(seen_indices[i] == false && mod_list[i].name_ == dep) {
				seen_indices[i] = true;
				if(transitively_depends_on_internal(mod_list[i], modb, seen_indices))
					return true;
			}
		}
	}
	return false;
}

bool transitively_depends_on(parsers::mod_file const& moda, parsers::mod_file const& modb) {
	std::vector<bool> seen_indices;
	seen_indices.resize(mod_list.size());

	return transitively_depends_on_internal(moda, modb, seen_indices);
}

void enforce_list_order() {
	std::stable_sort(mod_list.begin(), mod_list.end(), [&](parsers::mod_file const& a, parsers::mod_file const& b) {
		if(a.mod_selected && b.mod_selected) {
			return transitively_depends_on(b, a);
		} else if(a.mod_selected && !b.mod_selected) {
			return true;
		} else if(!a.mod_selected && b.mod_selected) {
			return false;
		} else {
			return a.name_ < b.name_;
		}
	});
}

bool nth_item_can_move_up(int32_t n) {
	if(n == 0)
		return false;
	if(transitively_depends_on(mod_list[n], mod_list[n - 1]))
		return false;

	return true;
}
bool nth_item_can_move_down(int32_t n) {
	if(n >= int32_t(mod_list.size()) - 1)
		return false;
	if(mod_list[n + 1].mod_selected == false)
		return false;
	if(transitively_depends_on(mod_list[n+1], mod_list[n]))
		return false;

	return true;
}

native_string produce_mod_path() {
	simple_fs::file_system dummy;
	simple_fs::add_root(dummy, NATIVE("."));

	for(int32_t i = 0; i < int32_t(mod_list.size()); ++i) {
		if(mod_list[i].mod_selected == false)
			break;

		mod_list[i].add_to_file_system(dummy);
	}

	return simple_fs::extract_state(dummy);
}

native_string to_hex(uint64_t v) {
	native_string ret;
	constexpr native_char digits[] = NATIVE("0123456789ABCDEF");
	do {
		ret += digits[v & 0x0F];
		v = v >> 4;
	} while(v != 0);

	return ret;
}

void make_mod_file() {
	file_is_ready.store(false, std::memory_order::memory_order_seq_cst);
	auto path = produce_mod_path();
	std::thread file_maker([path]() {
		auto game_state = std::make_unique<sys::state>();
		simple_fs::restore_state(game_state->common_fs, path);

		parsers::error_handler err("");
		game_state->load_scenario_data(err);

		auto sdir = simple_fs::get_or_create_scenario_directory();
		int32_t append = 0;

		auto time_stamp = uint64_t(std::time(0));
		auto base_name = to_hex(time_stamp);
		while(simple_fs::peek_file(sdir, base_name + NATIVE("-")  + std::to_wstring(append) + NATIVE(".bin"))) {
			++append;
		}

		++max_scenario_count;
		selected_scenario_file = base_name + NATIVE("-") + std::to_wstring(append) + NATIVE(".bin");
		sys::write_scenario_file(*game_state, selected_scenario_file, max_scenario_count);

		if(!err.accumulated_errors.empty() || !err.accumulated_warnings.empty()) {
			auto assembled_file = std::string("The following problems were encountered while creating the scenario:\r\n\r\nErrors:\r\n") + err.accumulated_errors + "\r\n\r\nWarnings:\r\n" + err.accumulated_warnings;
			auto pdir = simple_fs::get_or_create_settings_directory();
			simple_fs::write_file(pdir, NATIVE("scenario_errors.txt"), assembled_file.data(), uint32_t(assembled_file.length()));

			if(!err.accumulated_errors.empty()) {
				auto fname = simple_fs::get_full_name(pdir) + L"\\scenario_errors.txt";
				ShellExecuteW(
					nullptr,
					L"open",
					fname.c_str(),
					nullptr,
					nullptr,
					SW_NORMAL
				);
			}
		}
		if(!err.fatal) {
			auto of = simple_fs::open_file(sdir, selected_scenario_file);

			if(of) {
				auto content = view_contents(*of);
				auto desc = sys::extract_mod_information(reinterpret_cast<uint8_t const*>(content.data), content.file_size);
				if(desc.count != 0) {
					scenario_files.push_back(scenario_file{ selected_scenario_file , desc });
				}
			}
			

			std::sort(scenario_files.begin(), scenario_files.end(), [](scenario_file const& a, scenario_file const& b) {
				return a.ident.count > b.ident.count;
			});
		}

		file_is_ready.store(true, std::memory_order::memory_order_release);

		InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
	});

	file_maker.detach();
}

void find_scenario_file() {
	if(!file_is_ready.load(std::memory_order::memory_order_acquire))
		return;

	file_is_ready.store(false, std::memory_order::memory_order_seq_cst);
	selected_scenario_file = NATIVE("");
	auto mod_path = produce_mod_path();

	for(auto& f : scenario_files) {
		if(f.ident.mod_path == mod_path) {
			selected_scenario_file = f.file_name;
			break;
		}
	}
	file_is_ready.store(true, std::memory_order::memory_order_release);
}

void mouse_click() {
	if(obj_under_mouse == -1)
		return;

	switch(obj_under_mouse) {
		case ui_obj_close:
			PostMessageW(m_hwnd, WM_CLOSE, 0, 0);
			return;
		case ui_obj_list_left :
			if(frame_in_list > 0) {
				--frame_in_list;
				InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
			}
			return;
		case ui_obj_list_right:
			if((frame_in_list + 1) * ui_list_count < int32_t(mod_list.size())) {
				++frame_in_list;
				InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
			}
			return;
		case ui_obj_create_scenario:
			if(file_is_ready.load(std::memory_order::memory_order_acquire)) {
				make_mod_file();
				InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
			}
			return;
		case ui_obj_play_game:
		case ui_obj_host_game:
		case ui_obj_join_game:
			if(file_is_ready.load(std::memory_order::memory_order_acquire) && !selected_scenario_file.empty()) {
				native_string temp_command_line = native_string(NATIVE("Alice.exe ")) + selected_scenario_file;
				if(obj_under_mouse == ui_obj_host_game) {
					temp_command_line += NATIVE(" -host");
					temp_command_line += NATIVE(" -name ");
					temp_command_line += simple_fs::utf8_to_native(player_name);
				} else if(obj_under_mouse == ui_obj_join_game) {
					temp_command_line += NATIVE(" -join");
					temp_command_line += NATIVE(" ");
					temp_command_line += simple_fs::utf8_to_native(ip_addr);
					temp_command_line += NATIVE(" -name ");
					temp_command_line += simple_fs::utf8_to_native(player_name);
				}

				// IPv6 address
				if(!ip_addr.empty() && ::strchr(ip_addr.c_str(), ':') != NULL) {
					temp_command_line += NATIVE(" -v6");
				}

				STARTUPINFO si;
				ZeroMemory(&si, sizeof(si));
				si.cb = sizeof(si);
				PROCESS_INFORMATION pi;
				ZeroMemory(&pi, sizeof(pi));
				// Start the child process. 
				if(CreateProcessW(
					nullptr,   // Module name
					const_cast<wchar_t*>(temp_command_line.c_str()), // Command line
					nullptr, // Process handle not inheritable
					nullptr, // Thread handle not inheritable
					FALSE, // Set handle inheritance to FALSE
					0, // No creation flags
					nullptr, // Use parent's environment block
					nullptr, // Use parent's starting directory 
					&si, // Pointer to STARTUPINFO structure
					&pi) != 0) {

					CloseHandle(pi.hProcess);
					CloseHandle(pi.hThread);
				}
				

				// ready to launch
			}
			return;
		default:
			break;
	}

	if(!file_is_ready.load(std::memory_order::memory_order_acquire))
		return;

	int32_t list_position = (obj_under_mouse - ui_list_first) / 3;
	int32_t sub_obj = (obj_under_mouse - ui_list_first) - list_position * 3;

	switch(sub_obj) {
		case ui_list_checkbox:
		{
			int32_t list_offset = launcher::frame_in_list * launcher::ui_list_count + list_position;
			if(list_offset < int32_t(launcher::mod_list.size())) {
				launcher::mod_list[list_offset].mod_selected = !launcher::mod_list[list_offset].mod_selected;
				if(!launcher::mod_list[list_offset].mod_selected) {
					recursively_remove_from_list(launcher::mod_list[list_offset]);
				} else {
					recursively_add_to_list(launcher::mod_list[list_offset]);
				}
				enforce_list_order();
				find_scenario_file();
				InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
			}
			return;
		}
		case ui_list_move_up:
		{
			int32_t list_offset = launcher::frame_in_list * launcher::ui_list_count + list_position;
			if(launcher::mod_list[list_offset].mod_selected && nth_item_can_move_up(list_offset)) {
				std::swap(launcher::mod_list[list_offset], launcher::mod_list[list_offset - 1]);
				find_scenario_file();
				InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
			}
			return;
		}
		case ui_list_move_down:
		{
			int32_t list_offset = launcher::frame_in_list * launcher::ui_list_count + list_position;
			if(launcher::mod_list[list_offset].mod_selected && nth_item_can_move_down(list_offset)) {
				std::swap(launcher::mod_list[list_offset], launcher::mod_list[list_offset + 1]);
				find_scenario_file();
				InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
			}
			return;
		}
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

static GLuint ui_shader_program = 0;

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

static GLuint global_square_vao = 0;
static GLuint global_square_buffer = 0;
static GLuint global_square_right_buffer = 0;
static GLuint global_square_left_buffer = 0;
static GLuint global_square_flipped_buffer = 0;
static GLuint global_square_right_flipped_buffer = 0;
static GLuint global_square_left_flipped_buffer = 0;

static GLuint sub_square_buffers[64] = { 0 };

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

static ::text::font_manager font_collection;

static ::ogl::texture bg_tex;
static ::ogl::texture left_tex;
static ::ogl::texture right_tex;
static ::ogl::texture close_tex;
static ::ogl::texture big_button_tex;
static ::ogl::texture empty_check_tex;
static ::ogl::texture check_tex;
static ::ogl::texture up_tex;
static ::ogl::texture down_tex;
static ::ogl::texture line_bg_tex;


float base_text_extent(char const* codepoints, uint32_t count, int32_t size, text::font& fnt) {
	float total = 0.0f;
	for(uint32_t i = 0; i < count; i++) {
		auto c = uint8_t(codepoints[i]);
		total += fnt.glyph_advances[c] * size / 64.0f + ((i != 0) ? fnt.kerning(codepoints[i - 1], c) * size / 64.0f : 0.0f);
	}
	return total;
}

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

	glViewport(0, 0, int32_t(base_width * scaling_factor), int32_t(base_height * scaling_factor));
	glDepthRange(-1.0f, 1.0f);

	launcher::ogl::render_textured_rect(launcher::ogl::color_modification::none, 0.0f, 0.0f, base_width, base_height, bg_tex.get_texture_handle(), ui::rotation::upright, false);

	launcher::ogl::render_new_text("Project Alice", 13, launcher::ogl::color_modification::none, 78.0f, 5.0f, 26.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, font_collection.fonts[1]);

	launcher::ogl::render_textured_rect(obj_under_mouse == ui_obj_close ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
		ui_rects[ui_obj_close].x,
		ui_rects[ui_obj_close].y,
		ui_rects[ui_obj_close].width,
		ui_rects[ui_obj_close].height,
		close_tex.get_texture_handle(), ui::rotation::upright, false);

	if(int32_t(mod_list.size()) > ui_list_count) {
		if(frame_in_list > 0) {
			launcher::ogl::render_textured_rect(obj_under_mouse == ui_obj_list_left ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
				ui_rects[ui_obj_list_left].x,
				ui_rects[ui_obj_list_left].y,
				ui_rects[ui_obj_list_left].width,
				ui_rects[ui_obj_list_left].height,
				left_tex.get_texture_handle(), ui::rotation::upright, false);
		} else {
			launcher::ogl::render_textured_rect(launcher::ogl::color_modification::disabled,
				ui_rects[ui_obj_list_left].x,
				ui_rects[ui_obj_list_left].y,
				ui_rects[ui_obj_list_left].width,
				ui_rects[ui_obj_list_left].height,
				left_tex.get_texture_handle(), ui::rotation::upright, false);
		}

		if((frame_in_list + 1) * ui_list_count < int32_t(mod_list.size())) {
			launcher::ogl::render_textured_rect(obj_under_mouse == ui_obj_list_right ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
				ui_rects[ui_obj_list_right].x,
				ui_rects[ui_obj_list_right].y,
				ui_rects[ui_obj_list_right].width,
				ui_rects[ui_obj_list_right].height,
				right_tex.get_texture_handle(), ui::rotation::upright, false);
		} else {
			launcher::ogl::render_textured_rect(launcher::ogl::color_modification::disabled,
				ui_rects[ui_obj_list_right].x,
				ui_rects[ui_obj_list_right].y,
				ui_rects[ui_obj_list_right].width,
				ui_rects[ui_obj_list_right].height,
				right_tex.get_texture_handle(), ui::rotation::upright, false);
		}
	}

	if(file_is_ready.load(std::memory_order::memory_order_acquire)) {
		launcher::ogl::render_textured_rect(obj_under_mouse == ui_obj_create_scenario ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
			ui_rects[ui_obj_create_scenario].x,
			ui_rects[ui_obj_create_scenario].y,
			ui_rects[ui_obj_create_scenario].width,
			ui_rects[ui_obj_create_scenario].height,
			big_button_tex.get_texture_handle(), ui::rotation::upright, false);

		if(selected_scenario_file.empty()) {
			float x_pos = ui_rects[ui_obj_create_scenario].x + ui_rects[ui_obj_create_scenario].width / 2 - base_text_extent("Create Scenario", 15, 22, font_collection.fonts[1]) / 2.0f;
			launcher::ogl::render_new_text("Create Scenario", 15, launcher::ogl::color_modification::none, x_pos, ui_rects[ui_obj_create_scenario].y + 2.f, 22.0f, launcher::ogl::color3f{ 50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f }, font_collection.fonts[1]);
		} else {
			float x_pos = ui_rects[ui_obj_create_scenario].x + ui_rects[ui_obj_create_scenario].width / 2 - base_text_extent("Recreate Scenario", 17, 22, font_collection.fonts[1]) / 2.0f;
			launcher::ogl::render_new_text("Recreate Scenario", 17, launcher::ogl::color_modification::none, x_pos, ui_rects[ui_obj_create_scenario].y + 2.f, 22.0f, launcher::ogl::color3f{ 50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f }, font_collection.fonts[1]);
		}
	} else {
		launcher::ogl::render_textured_rect(launcher::ogl::color_modification::disabled,
			ui_rects[ui_obj_create_scenario].x,
			ui_rects[ui_obj_create_scenario].y,
			ui_rects[ui_obj_create_scenario].width,
			ui_rects[ui_obj_create_scenario].height,
			big_button_tex.get_texture_handle(), ui::rotation::upright, false);

		float x_pos = ui_rects[ui_obj_create_scenario].x + ui_rects[ui_obj_create_scenario].width / 2 - base_text_extent("Working...", 10, 22, font_collection.fonts[1]) / 2.0f;
		launcher::ogl::render_new_text("Working...", 10, launcher::ogl::color_modification::none, x_pos, 50.0f, 22.0f, launcher::ogl::color3f{ 50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f }, font_collection.fonts[1]);

		
	}

	{
		/*
		Create a new scenario file
		for the selected mods
		*/
		auto xoffset = 830.0f - base_text_extent("Create a new scenario file", 26, 14, font_collection.fonts[0]);
		launcher::ogl::render_new_text("Create a new scenario file", 26, launcher::ogl::color_modification::none, xoffset, 94.0f + 0 * 18.0f, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, font_collection.fonts[0]);

		xoffset = 830.0f - base_text_extent("for the selected mods", 21, 14, font_collection.fonts[0]);
		launcher::ogl::render_new_text("for the selected mods", 21, launcher::ogl::color_modification::none, xoffset, 94.0f + 1 * 18.0f, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, font_collection.fonts[0]);
	}

	if(file_is_ready.load(std::memory_order::memory_order_acquire) && !selected_scenario_file.empty()) {
		launcher::ogl::render_textured_rect(obj_under_mouse == ui_obj_play_game ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
			ui_rects[ui_obj_play_game].x,
			ui_rects[ui_obj_play_game].y,
			ui_rects[ui_obj_play_game].width,
			ui_rects[ui_obj_play_game].height,
			big_button_tex.get_texture_handle(), ui::rotation::upright, false);
		launcher::ogl::render_textured_rect(obj_under_mouse == ui_obj_host_game ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
			ui_rects[ui_obj_host_game].x,
			ui_rects[ui_obj_host_game].y,
			ui_rects[ui_obj_host_game].width,
			ui_rects[ui_obj_host_game].height,
			big_button_tex.get_texture_handle(), ui::rotation::upright, false);
		launcher::ogl::render_textured_rect(obj_under_mouse == ui_obj_join_game ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
			ui_rects[ui_obj_join_game].x,
			ui_rects[ui_obj_join_game].y,
			ui_rects[ui_obj_join_game].width,
			ui_rects[ui_obj_join_game].height,
			big_button_tex.get_texture_handle(), ui::rotation::upright, false);
	} else {
		launcher::ogl::render_textured_rect(launcher::ogl::color_modification::disabled,
			ui_rects[ui_obj_play_game].x,
			ui_rects[ui_obj_play_game].y,
			ui_rects[ui_obj_play_game].width,
			ui_rects[ui_obj_play_game].height,
			big_button_tex.get_texture_handle(), ui::rotation::upright, false);
		launcher::ogl::render_textured_rect(launcher::ogl::color_modification::disabled,
			ui_rects[ui_obj_host_game].x,
			ui_rects[ui_obj_host_game].y,
			ui_rects[ui_obj_host_game].width,
			ui_rects[ui_obj_host_game].height,
			big_button_tex.get_texture_handle(), ui::rotation::upright, false);
		launcher::ogl::render_textured_rect(launcher::ogl::color_modification::disabled,
			ui_rects[ui_obj_join_game].x,
			ui_rects[ui_obj_join_game].y,
			ui_rects[ui_obj_join_game].width,
			ui_rects[ui_obj_join_game].height,
			big_button_tex.get_texture_handle(), ui::rotation::upright, false);

		/*830, 250*/
		// No scenario file found

		auto xoffset = 830.0f - base_text_extent("No scenario file found", 22, 14, font_collection.fonts[0]);
		launcher::ogl::render_new_text("No scenario file found", 22, launcher::ogl::color_modification::none, xoffset, ui_rects[ui_obj_play_game].y + 48.f, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, font_collection.fonts[0]);
	}

	launcher::ogl::render_new_text("IP Address", 10, launcher::ogl::color_modification::none, ui_rects[ui_obj_ip_addr].x + ui_rects[ui_obj_ip_addr].width - base_text_extent("IP Address", 10, 14, font_collection.fonts[0]), ui_rects[ui_obj_ip_addr].y - 18.f, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, font_collection.fonts[0]);
	launcher::ogl::render_textured_rect(obj_under_mouse == ui_obj_ip_addr ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
		ui_rects[ui_obj_ip_addr].x,
		ui_rects[ui_obj_ip_addr].y,
		ui_rects[ui_obj_ip_addr].width,
		ui_rects[ui_obj_ip_addr].height,
		line_bg_tex.get_texture_handle(), ui::rotation::upright, false);
	launcher::ogl::render_new_text("Nickname", 8, launcher::ogl::color_modification::none, ui_rects[ui_obj_player_name].x + ui_rects[ui_obj_player_name].width - base_text_extent("Nickname", 8, 14, font_collection.fonts[0]), ui_rects[ui_obj_player_name].y - 18.f, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, font_collection.fonts[0]);
	launcher::ogl::render_textured_rect(obj_under_mouse == ui_obj_player_name ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
		ui_rects[ui_obj_player_name].x,
		ui_rects[ui_obj_player_name].y,
		ui_rects[ui_obj_player_name].width,
		ui_rects[ui_obj_player_name].height,
		line_bg_tex.get_texture_handle(), ui::rotation::upright, false);

	launcher::ogl::render_new_text("Singleplayer", 13, launcher::ogl::color_modification::none, ui_rects[ui_obj_play_game].x + ui_rects[ui_obj_play_game].width - base_text_extent("Singleplayer", 13, 22, font_collection.fonts[1]), ui_rects[ui_obj_play_game].y - 32.f, 22.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, font_collection.fonts[1]);

	float sg_x_pos = ui_rects[ui_obj_play_game].x + ui_rects[ui_obj_play_game].width / 2 - base_text_extent("Start game", 10, 22, font_collection.fonts[1]) / 2.0f;
	launcher::ogl::render_new_text("Start game", 10, launcher::ogl::color_modification::none, sg_x_pos, ui_rects[ui_obj_play_game].y + 2.f, 22.0f, launcher::ogl::color3f{ 50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f }, font_collection.fonts[1]);

	const char* hg_text = "Host";
	const char* jg_text = "Join";
	if(!ip_addr.empty() && ::strchr(ip_addr.c_str(), ':') != NULL) {
		hg_text = "Host<IPv6>";
		jg_text = "Join<IPv6>";
	}

	launcher::ogl::render_new_text("Multiplayer", 12, launcher::ogl::color_modification::none, ui_rects[ui_obj_join_game].x + ui_rects[ui_obj_join_game].width - base_text_extent("Multiplayer", 12, 22, font_collection.fonts[1]), ui_rects[ui_obj_host_game].y - 32.f, 22.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, font_collection.fonts[1]);

	// Join and host game buttons
	float hg_x_pos = ui_rects[ui_obj_host_game].x + ui_rects[ui_obj_host_game].width / 2 - base_text_extent(hg_text, uint32_t(::strlen(hg_text)), 22, font_collection.fonts[1]) / 2.0f;
	launcher::ogl::render_new_text(hg_text, uint32_t(::strlen(hg_text)), launcher::ogl::color_modification::none, hg_x_pos, ui_rects[ui_obj_host_game].y + 2.f, 22.0f, launcher::ogl::color3f{ 50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f }, font_collection.fonts[1]);
	float jg_x_pos = ui_rects[ui_obj_join_game].x + ui_rects[ui_obj_join_game].width / 2 - base_text_extent(jg_text, uint32_t(::strlen(jg_text)), 22, font_collection.fonts[1]) / 2.0f;
	launcher::ogl::render_new_text(jg_text, uint32_t(::strlen(jg_text)), launcher::ogl::color_modification::none, jg_x_pos, ui_rects[ui_obj_join_game].y + 2.f, 22.0f, launcher::ogl::color3f{ 50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f }, font_collection.fonts[1]);

	// Text fields
	float ia_x_pos = ui_rects[ui_obj_ip_addr].x + 4.f;// ui_rects[ui_obj_ip_addr].width - base_text_extent(ip_addr.c_str(), uint32_t(ip_addr.length()), 14, font_collection.fonts[0]) - 4.f;
	launcher::ogl::render_new_text(ip_addr.c_str(), uint32_t(ip_addr.size()), launcher::ogl::color_modification::none, ia_x_pos, ui_rects[ui_obj_ip_addr].y + 5.f, 14.0f, launcher::ogl::color3f{ 255.0f, 255.0f, 255.0f }, font_collection.fonts[0]);
	float pn_x_pos = ui_rects[ui_obj_player_name].x + 4.f;// ui_rects[ui_obj_player_name].width - base_text_extent(player_name.c_str(), uint32_t(player_name.length()), 14, font_collection.fonts[0]) - 4.f;
	launcher::ogl::render_new_text(player_name.c_str(), uint32_t(player_name.size()), launcher::ogl::color_modification::none, pn_x_pos, ui_rects[ui_obj_player_name].y + 5.f, 14.0f, launcher::ogl::color3f{ 255.0f, 255.0f, 255.0f }, font_collection.fonts[0]);

	auto ml_xoffset = list_text_right_align - base_text_extent("Mod List", 8, 24, font_collection.fonts[1]);
	launcher::ogl::render_new_text("Mod List", 8, launcher::ogl::color_modification::none, ml_xoffset, 45.0f, 24.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, font_collection.fonts[1]);

	
	int32_t list_offset = launcher::frame_in_list * launcher::ui_list_count;

	for(int32_t i = 0; i < ui_list_count && list_offset + i < int32_t(mod_list.size()); ++i) {
		auto& mod_ref = mod_list[list_offset + i];

		if(i % 2 == 1) {
			launcher::ogl::render_textured_rect(
				launcher::ogl::color_modification::none,
				60.0f,
				75.0f + float(ui_row_height * i),
				440.0f,
				float(ui_row_height),
				launcher::line_bg_tex.get_texture_handle(), ui::rotation::upright, false);
		}

		if(mod_ref.mod_selected) {
			launcher::ogl::render_textured_rect(obj_under_mouse == ui_list_first + 3 * i + ui_list_checkbox ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
			ui_rects[ui_list_first + 3 * i + ui_list_checkbox].x,
			ui_rects[ui_list_first + 3 * i + ui_list_checkbox].y,
			ui_rects[ui_list_first + 3 * i + ui_list_checkbox].width,
			ui_rects[ui_list_first + 3 * i + ui_list_checkbox].height,
			check_tex.get_texture_handle(), ui::rotation::upright, false);

			if(!nth_item_can_move_up(list_offset + i)) {
				launcher::ogl::render_textured_rect(launcher::ogl::color_modification::disabled,
					ui_rects[ui_list_first + 3 * i + ui_list_move_up].x,
					ui_rects[ui_list_first + 3 * i + ui_list_move_up].y,
					ui_rects[ui_list_first + 3 * i + ui_list_move_up].width,
					ui_rects[ui_list_first + 3 * i + ui_list_move_up].height,
					up_tex.get_texture_handle(), ui::rotation::upright, false);
			} else {
				launcher::ogl::render_textured_rect(obj_under_mouse == ui_list_first + 3 * i + ui_list_move_up ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
					ui_rects[ui_list_first + 3 * i + ui_list_move_up].x,
					ui_rects[ui_list_first + 3 * i + ui_list_move_up].y,
					ui_rects[ui_list_first + 3 * i + ui_list_move_up].width,
					ui_rects[ui_list_first + 3 * i + ui_list_move_up].height,
					up_tex.get_texture_handle(), ui::rotation::upright, false);
			}
			if(!nth_item_can_move_down(list_offset + i)) {
				launcher::ogl::render_textured_rect(launcher::ogl::color_modification::disabled,
					ui_rects[ui_list_first + 3 * i + ui_list_move_down].x,
					ui_rects[ui_list_first + 3 * i + ui_list_move_down].y,
					ui_rects[ui_list_first + 3 * i + ui_list_move_down].width,
					ui_rects[ui_list_first + 3 * i + ui_list_move_down].height,
					down_tex.get_texture_handle(), ui::rotation::upright, false);
			} else {
				launcher::ogl::render_textured_rect(obj_under_mouse == ui_list_first + 3 * i + ui_list_move_down ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
					ui_rects[ui_list_first + 3 * i + ui_list_move_down].x,
					ui_rects[ui_list_first + 3 * i + ui_list_move_down].y,
					ui_rects[ui_list_first + 3 * i + ui_list_move_down].width,
					ui_rects[ui_list_first + 3 * i + ui_list_move_down].height,
					down_tex.get_texture_handle(), ui::rotation::upright, false);
			}
		} else {
			launcher::ogl::render_textured_rect(obj_under_mouse == ui_list_first + 3 * i + ui_list_checkbox ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
			ui_rects[ui_list_first + 3 * i + ui_list_checkbox].x,
			ui_rects[ui_list_first + 3 * i + ui_list_checkbox].y,
			ui_rects[ui_list_first + 3 * i + ui_list_checkbox].width,
			ui_rects[ui_list_first + 3 * i + ui_list_checkbox].height,
			empty_check_tex.get_texture_handle(), ui::rotation::upright, false);
		}

		auto xoffset = list_text_right_align - base_text_extent(mod_ref.name_.data(), uint32_t(mod_ref.name_.length()), 14, font_collection.fonts[0]);

		launcher::ogl::render_new_text(mod_ref.name_.data(), uint32_t(mod_ref.name_.length()), launcher::ogl::color_modification::none, xoffset, 75.0f + 7.0f + i * ui_row_height, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, font_collection.fonts[0]);
	}

	SwapBuffers(opengl_window_dc);
}

bool is_low_surrogate(uint16_t char_code) noexcept {
	return char_code >= 0xDC00 && char_code <= 0xDFFF;
}
bool is_high_surrogate(uint16_t char_code) noexcept {
	return char_code >= 0xD800 && char_code <= 0xDBFF;
}

char process_utf16_to_win1250(wchar_t c) {
	if(c <= 127)
		return char(c);
	if(is_low_surrogate(c) || is_high_surrogate(c))
		return 0;
	char char_out = 0;
	WideCharToMultiByte(1250, 0, &c, 1, &char_out, 1, nullptr, nullptr);
	return char_out;
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
		add_root(fs, NATIVE_M(GAME_DIR));
		simple_fs::add_root(fs, NATIVE("."));
		auto root = get_root(fs);

		auto font_a = open_file(root, NATIVE("assets/fonts/LibreCaslonText-Regular.ttf"));
		if(font_a) {
			auto file_content = view_contents(*font_a);
			font_collection.load_font(font_collection.fonts[0], file_content.data, file_content.file_size, text::font_feature::none);
		}
		auto font_b = open_file(root, NATIVE("assets/fonts/LibreCaslonText-Italic.ttf"));
		if(font_b) {
			auto file_content = view_contents(*font_b);
			font_collection.load_font(font_collection.fonts[1], file_content.data, file_content.file_size, text::font_feature::none);
		}

		font_collection.load_all_glyphs();

		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_bg.png"), fs, bg_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_left.png"), fs, left_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_right.png"), fs, right_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_close.png"), fs, close_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_big_button.png"), fs, big_button_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_no_check.png"), fs, empty_check_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_check.png"), fs, check_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_up.png"), fs, up_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_down.png"), fs, down_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_line_bg.png"), fs, line_bg_tex, false);

		auto mod_dir = simple_fs::open_directory(root, NATIVE("mod"));
		auto mod_files = simple_fs::list_files(mod_dir, NATIVE(".mod"));

		parsers::error_handler err("");
		for(auto& f : mod_files) {
			auto of = simple_fs::open_file(f);
			if(of) {
				auto content = view_contents(*of);
				parsers::token_generator gen(content.data, content.data + content.file_size);
				mod_list.push_back(parsers::parse_mod_file(gen, err, parsers::mod_file_context{}));
			}
		}

		auto sdir = simple_fs::get_or_create_scenario_directory();
		auto s_files = simple_fs::list_files(sdir, NATIVE(".bin"));
		for(auto& f : s_files) {
			auto of = simple_fs::open_file(f);
			if(of) {
				auto content = view_contents(*of);
				auto desc = sys::extract_mod_information(reinterpret_cast<uint8_t const*>(content.data), content.file_size);
				if(desc.count != 0) {
					max_scenario_count = std::max(desc.count, max_scenario_count);
					scenario_files.push_back(scenario_file{simple_fs::get_file_name(f) , desc });
				}
			}
		}

		std::sort(scenario_files.begin(), scenario_files.end(), [](scenario_file const& a, scenario_file const& b) {
			return a.ident.count > b.ident.count;
		});

		find_scenario_file();

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

				mouse_x = int32_t(float(GET_X_LPARAM(lParam)) / scaling_factor);
				mouse_y = int32_t(float(GET_Y_LPARAM(lParam)) / scaling_factor);
				if(update_under_mouse()) {
					InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
				}
				return 0;
			}
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
			case WM_NCCREATE:
			{
				HINSTANCE hUser32dll = LoadLibrary(L"User32.dll");
				if(hUser32dll) {
					auto pSetProcessDpiAwarenessContext = (decltype(&SetProcessDpiAwarenessContext))GetProcAddress(hUser32dll, "SetProcessDpiAwarenessContext");
					if(pSetProcessDpiAwarenessContext == NULL) {
						// not present, so have to call this
						auto pEnableNonClientDpiScaling = (decltype(&EnableNonClientDpiScaling))GetProcAddress(hUser32dll, "EnableNonClientDpiScaling");
						if(pEnableNonClientDpiScaling != NULL) {
							pEnableNonClientDpiScaling(hwnd); //windows 10
						}
						FreeLibrary(hUser32dll);
					}
				}
				break;
			}
			case WM_CHAR:
			{
				char turned_into = process_utf16_to_win1250(wParam);
				if(turned_into) {
					if(obj_under_mouse == ui_obj_ip_addr) {
						if(turned_into == '\b') {
							if(!ip_addr.empty())
								ip_addr.pop_back();
						} else if(turned_into != '\t' && turned_into != ' ' && ip_addr.size() < 32) {
							ip_addr.push_back(turned_into);
						}
					} else if(obj_under_mouse == ui_obj_player_name) {
						if(turned_into == '\b') {
							if(!player_name.empty())
								player_name.pop_back();
						} else if(turned_into != '\t' && turned_into != ' ' && player_name.size() < 32) {
							player_name.push_back(turned_into);
						}
					}
				}
				InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
				return 0;
			}
			default:
				break;

		}
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
}

} // end launcher namespace

static CRITICAL_SECTION guard_abort_handler;

void signal_abort_handler(int) {
	static bool run_once = false;

	EnterCriticalSection(&guard_abort_handler);
	if(run_once == false) {
		run_once = true;

		STARTUPINFO si;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		PROCESS_INFORMATION pi;
		ZeroMemory(&pi, sizeof(pi));
		// Start the child process. 
		if(CreateProcessW(
			L"dbg_alice.exe",   // Module name
			NULL, // Command line
			NULL, // Process handle not inheritable
			NULL, // Thread handle not inheritable
			FALSE, // Set handle inheritance to FALSE
			0, // No creation flags
			NULL, // Use parent's environment block
			NULL, // Use parent's starting directory 
			&si, // Pointer to STARTUPINFO structure
			&pi) == 0) {

			// create process failed
			LeaveCriticalSection(&guard_abort_handler);
			return;

		}
		// Wait until child process exits.
		WaitForSingleObject(pi.hProcess, INFINITE);
		// Close process and thread handles. 
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	LeaveCriticalSection(&guard_abort_handler);
}

LONG WINAPI uef_wrapper(struct _EXCEPTION_POINTERS* lpTopLevelExceptionFilter) {
	signal_abort_handler(0);
	return EXCEPTION_CONTINUE_SEARCH;
}

void generic_wrapper() {
	signal_abort_handler(0);
}
void invalid_parameter_wrapper(
   const wchar_t* expression,
   const wchar_t* function,
   const wchar_t* file,
   unsigned int line,
   uintptr_t pReserved
) {
	signal_abort_handler(0);
}

void EnableCrashingOnCrashes() {
	typedef BOOL(WINAPI* tGetPolicy)(LPDWORD lpFlags);
	typedef BOOL(WINAPI* tSetPolicy)(DWORD dwFlags);
	const DWORD EXCEPTION_SWALLOWING = 0x1;

	HMODULE kernel32 = LoadLibraryA("kernel32.dll");
	tGetPolicy pGetPolicy = (tGetPolicy)GetProcAddress(kernel32, "GetProcessUserModeExceptionPolicy");
	tSetPolicy pSetPolicy = (tSetPolicy)GetProcAddress(kernel32, "SetProcessUserModeExceptionPolicy");
	if(pGetPolicy && pSetPolicy) {
		DWORD dwFlags;
		if(pGetPolicy(&dwFlags)) {
			// Turn off the filter
			pSetPolicy(dwFlags & ~EXCEPTION_SWALLOWING);
		}
	}
	BOOL insanity = FALSE;
	SetUserObjectInformationA(GetCurrentProcess(), UOI_TIMERPROC_EXCEPTION_SUPPRESSION, &insanity, sizeof(insanity));
}

int WINAPI wWinMain(
	HINSTANCE /*hInstance*/,
	HINSTANCE /*hPrevInstance*/,
	LPWSTR /*lpCmdLine*/,
	int /*nCmdShow*/
) {
#ifdef _DEBUG
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
#endif

	InitializeCriticalSection(&guard_abort_handler);

	if(!IsDebuggerPresent()) {
		EnableCrashingOnCrashes();
		_set_purecall_handler(generic_wrapper);
		_set_invalid_parameter_handler(invalid_parameter_wrapper);
		_set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
		SetUnhandledExceptionFilter(uef_wrapper);
		signal(SIGABRT, signal_abort_handler);
	}

	// Workaround for old machines
	HINSTANCE hUser32dll = LoadLibrary(L"User32.dll");
	if(hUser32dll) {
		auto pSetProcessDpiAwarenessContext = (decltype(&SetProcessDpiAwarenessContext))GetProcAddress(hUser32dll, "SetProcessDpiAwarenessContext");
		if(pSetProcessDpiAwarenessContext != NULL) {
			pSetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
		} else {
			// windows 8.1 (not present on windows 8 and only available on desktop apps)
			HINSTANCE hShcoredll = LoadLibrary(L"Shcore.dll");
			if(hShcoredll) {
				auto pSetProcessDpiAwareness = (decltype(&SetProcessDpiAwareness))GetProcAddress(hShcoredll, "SetProcessDpiAwareness");
				if(pSetProcessDpiAwareness != NULL) {
					pSetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
				} else {
					SetProcessDPIAware(); //vista+
				}
				FreeLibrary(hShcoredll);
			} else {
				SetProcessDPIAware(); //vista+
			}
		}
		FreeLibrary(hUser32dll);
	}

	if(!SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
		return 0;

	WNDCLASSEX wcex = { };
	wcex.cbSize = UINT(sizeof(WNDCLASSEX));
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

	// Use by default the name of the computer
	char username[256 + 1];
	DWORD username_len = 256 + 1;
	GetUserNameA(username, &username_len);
	launcher::player_name = std::string(reinterpret_cast<const char*>(&username[0]));
	//

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
		hUser32dll = LoadLibrary(L"User32.dll");
		if(hUser32dll) {
			auto pGetDpiForWindow = (decltype(&GetDpiForWindow))GetProcAddress(hUser32dll, "GetDpiForWindow");
			if(pGetDpiForWindow != NULL) {
				launcher::dpi = float(pGetDpiForWindow((HWND)(launcher::m_hwnd)));
			} else {
				launcher::dpi = 96.0f; //100%, default
			}
			FreeLibrary(hUser32dll);
		}

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
		TranslateMessage(&msg);
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
extern "C" {
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
};
