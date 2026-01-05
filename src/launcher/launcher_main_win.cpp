#ifndef UNICODE
#define UNICODE
#endif
#include <Windowsx.h>
#include <shellapi.h>
#include "Objbase.h"

#include "wglew.h"
#include <cassert>
#include "resource.h"
#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "icu.lib")

#include "launcher_main.hpp"

namespace launcher {

static HWND m_hwnd = nullptr;

#define ID_CURSOR_TIMER 1

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

	if(glewInit() != 0) {
		window::emit_error_message("GLEW failed to initialize", true);
	}

	if(!wglewIsSupported("WGL_ARB_create_context")) {
		window::emit_error_message("WGL_ARB_create_context not supported", true);
	}

	// Explicitly request for OpenGL 3.1
	static const int attribs_3_1[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 1,
		WGL_CONTEXT_FLAGS_ARB, 0,
		WGL_CONTEXT_PROFILE_MASK_ARB,
		WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};
	opengl_context = wglCreateContextAttribsARB(window_dc, nullptr, attribs_3_1);
	if(opengl_context == nullptr) {
		window::emit_error_message("Unable to create WGL context", true);
	}

	wglMakeCurrent(window_dc, HGLRC(opengl_context));
	wglDeleteContext(handle_to_ogl_dc);

	if(wglewIsSupported("WGL_EXT_swap_control_tear") == 1) {
		wglSwapIntervalEXT(-1);
	} else if(wglewIsSupported("WGL_EXT_swap_control") == 1) {
		wglSwapIntervalEXT(1);
	} else {
		window::emit_error_message("WGL_EXT_swap_control_tear and WGL_EXT_swap_control not supported", true);
	}
}

void shutdown_opengl() {
	wglMakeCurrent(opengl_window_dc, nullptr);
	wglDeleteContext(HGLRC(opengl_context));
	opengl_context = nullptr;
}

void make_mod_file() {
	file_is_ready.store(false, std::memory_order::memory_order_seq_cst);
	auto path = produce_mod_path();
	std::thread file_maker([path]() {
		simple_fs::file_system fs_root;
		simple_fs::restore_state(fs_root, path);
		parsers::error_handler err("");
		auto root = get_root(fs_root);
		auto common = open_directory(root, NATIVE("common"));
		auto save_dir = simple_fs::get_mod_save_dir_name(fs_root);
		parsers::bookmark_context bookmark_context;
		if(auto f = open_file(common, NATIVE("bookmarks.txt")); f) {
			auto bookmark_content = simple_fs::view_contents(*f);
			err.file_name = "bookmarks.txt";
			parsers::token_generator gen(bookmark_content.data, bookmark_content.data + bookmark_content.file_size);
			parsers::parse_bookmark_file(gen, err, bookmark_context);
			assert(!bookmark_context.bookmark_dates.empty());
		} else {
			err.accumulated_errors += "File common/bookmarks.txt could not be opened\n";
		}

		sys::checksum_key scenario_key;

		for(uint32_t date_index = 0; date_index < uint32_t(bookmark_context.bookmark_dates.size()); date_index++) {
			err.accumulated_errors.clear();
			err.accumulated_warnings.clear();
			//
			auto game_state = std::make_unique<sys::state>();
			game_state->mod_save_dir = save_dir;
			simple_fs::restore_state(game_state->common_fs, path);
			game_state->load_scenario_data(err, bookmark_context.bookmark_dates[date_index].date_);
			if(err.fatal)
				break;
			if(date_index == 0) {
				auto sdir = simple_fs::get_or_create_scenario_directory();
				int32_t append = 0;
				auto time_stamp = uint64_t(std::time(0));
				auto base_name = to_hex(time_stamp);
				auto generated_scenario_name = base_name + NATIVE("-") + std::to_wstring(append) + NATIVE(".bin");
				while(simple_fs::peek_file(sdir, generated_scenario_name)) {
					++append;
				}
				// In this case we override the file
				if(requestedScenarioFileName != "") {
					generated_scenario_name = simple_fs::utf8_to_native(requestedScenarioFileName);
				}
				++max_scenario_count;
				selected_scenario_file = generated_scenario_name;
				sys::write_scenario_file(*game_state, selected_scenario_file, max_scenario_count);
				if(auto of = simple_fs::open_file(sdir, selected_scenario_file); of) {
					auto content = view_contents(*of);
					auto desc = sys::extract_mod_information(reinterpret_cast<uint8_t const*>(content.data), content.file_size);
					if(desc.count != 0) {
						scenario_files.push_back(scenario_file{ selected_scenario_file , desc });
					}
				}
				std::sort(scenario_files.begin(), scenario_files.end(), [](scenario_file const& a, scenario_file const& b) {
					return a.ident.count > b.ident.count;
				});
				scenario_key = game_state->scenario_checksum;
			} else {
#ifndef NDEBUG
				sys::write_scenario_file(*game_state, std::to_wstring(date_index) + NATIVE(".bin"), 0);
#endif
				game_state->scenario_checksum = scenario_key;
				sys::write_save_file(*game_state, sys::save_type::bookmark, bookmark_context.bookmark_dates[date_index].name_);
			}
		}

		if(!err.accumulated_errors.empty() || !err.accumulated_warnings.empty()) {
			auto assembled_file = std::string("You can still play the mod, but it might be unstable\r\nThe following problems were encountered while creating the scenario:\r\n\r\nErrors:\r\n") + err.accumulated_errors + "\r\n\r\nWarnings:\r\n" + err.accumulated_warnings;
			auto pdir = simple_fs::get_or_create_settings_directory();
			simple_fs::write_file(pdir, NATIVE("scenario_errors.txt"), assembled_file.data(), uint32_t(assembled_file.length()));

			if(!err.accumulated_errors.empty()) {
				auto fname = simple_fs::get_full_name(pdir) + NATIVE("\\scenario_errors.txt");
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
		file_is_ready.store(true, std::memory_order::memory_order_release);

		if(autoBuild) {
			PostMessageW(m_hwnd, WM_CLOSE, 0, 0);
		}
		else {
			InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
		}
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

void set_cursor() {
	if(obj_under_mouse == active_textbox) {
		is_cursor_visible = true;
		SetTimer((HWND)(m_hwnd), ID_CURSOR_TIMER, 500, nullptr);
	} else {
		active_textbox = -1;
		is_cursor_visible = false;
		KillTimer((HWND)(m_hwnd), ID_CURSOR_TIMER);
	}
	InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
}

void mouse_click() {
	if(obj_under_mouse == -1)
		return;

	switch(obj_under_mouse) {
	case ui_obj_close:
		PostMessageW(m_hwnd, WM_CLOSE, 0, 0);
		return;
	case ui_obj_list_left:
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
		if(file_is_ready.load(std::memory_order::memory_order_acquire) && !selected_scenario_file.empty()) {
			if(IsProcessorFeaturePresent(PF_AVX512F_INSTRUCTIONS_AVAILABLE)) {
				native_string temp_command_line = native_string(NATIVE("Alice512.exe ")) + selected_scenario_file;

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

					return; // exit -- don't try starting avx2
				}
			}
			if(!IsProcessorFeaturePresent(PF_AVX2_INSTRUCTIONS_AVAILABLE)) {
				native_string temp_command_line = native_string(NATIVE("AliceSSE.exe ")) + selected_scenario_file;

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

					return; // exit -- don't try starting avx2
				}
			}
			{ // normal case (avx2)
				native_string temp_command_line = native_string(NATIVE("Alice.exe ")) + selected_scenario_file;

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
				return;
			}
		}
		break;
	case ui_obj_host_game:
		[[fallthrough]];
	case ui_obj_join_game:
		if(file_is_ready.load(std::memory_order::memory_order_acquire) && !selected_scenario_file.empty()) {
			native_string temp_command_line = native_string(NATIVE("AliceSSE.exe ")) + selected_scenario_file;
			if(obj_under_mouse == ui_obj_host_game) {
				temp_command_line += NATIVE(" -host");
				temp_command_line += NATIVE(" -name ");
				temp_command_line += simple_fs::utf8_to_native(player_name.to_string());

				if (!player_password.empty()) {
					temp_command_line += NATIVE(" -player_password ");
					temp_command_line += simple_fs::utf8_to_native(player_password.to_string());
				}

			} else if(obj_under_mouse == ui_obj_join_game) {
				temp_command_line += NATIVE(" -join");
				temp_command_line += NATIVE(" ");
				temp_command_line += simple_fs::utf8_to_native(ip_addr);
				temp_command_line += NATIVE(" -name ");
				temp_command_line += simple_fs::utf8_to_native(player_name.to_string());

				if (!player_password.empty()) {
					temp_command_line += NATIVE(" -player_password ");
					temp_command_line += simple_fs::utf8_to_native(player_password.to_string());
				}

				// IPv6 address
				if(!ip_addr.empty() && ::strchr(ip_addr.c_str(), ':') != nullptr) {
					temp_command_line += NATIVE(" -v6");
				}
			}

			if(!lobby_password.empty()) {
				temp_command_line += NATIVE(" -password ");
				temp_command_line += simple_fs::utf8_to_native(lobby_password);
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
	case ui_obj_ip_addr:
		active_textbox = ui_obj_ip_addr;
		return;
	case ui_obj_password:
		active_textbox = ui_obj_password;
		return;
	case ui_obj_player_password:
		active_textbox = ui_obj_player_password;
		return;
	case ui_obj_player_name:
		active_textbox = ui_obj_player_name;
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
		"#version 140\r\n",
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

void load_shaders() {
	simple_fs::file_system fs;
	simple_fs::add_root(fs, NATIVE("."));
	auto root = get_root(fs);

	std::string_view fx_str =
		"in vec2 tex_coord;\n"
		"out vec4 frag_color;\n"
		"uniform sampler2D texture_sampler;\n"
		"uniform vec4 d_rect;\n"
		"uniform float border_size;\n"
		"uniform vec3 inner_color;\n"
		"uniform vec4 subrect;\n"
		"uniform uvec2 subroutines_index;\n"
		"vec4 color_filter(vec2 tc) {\n"
		"\tvec4 color_in = texture(texture_sampler, tc);\n"
		"\tfloat sm_val = smoothstep(0.5 - border_size / 2.0, 0.5 + border_size / 2.0, color_in.r);\n"
		"\treturn vec4(inner_color, sm_val);\n"
		"}\n"
		"vec4 no_filter(vec2 tc) {\n"
		"\treturn texture(texture_sampler, tc);\n"
		"}\n"
		"vec4 disabled_color(vec4 color_in) {\n"
		"\tfloat amount = (color_in.r + color_in.g + color_in.b) / 4.0;\n"
		"\treturn vec4(amount, amount, amount, color_in.a);\n"
		"}\n"
		"vec4 interactable_color(vec4 color_in) {\n"
		"\treturn vec4(color_in.r + 0.1, color_in.g + 0.1, color_in.b + 0.1, color_in.a);\n"
		"}\n"
		"vec4 interactable_disabled_color(vec4 color_in) {\n"
		"\tfloat amount = (color_in.r + color_in.g + color_in.b) / 4.0;\n"
		"\treturn vec4(amount + 0.1, amount + 0.1, amount + 0.1, color_in.a);\n"
		"}\n"
		"vec4 tint_color(vec4 color_in) {\n"
		"\treturn vec4(color_in.r * inner_color.r, color_in.g * inner_color.g, color_in.b * inner_color.b, color_in.a);\n"
		"}\n"
		"vec4 enabled_color(vec4 color_in) {\n"
		"\treturn color_in;\n"
		"}\n"
		"vec4 alt_tint_color(vec4 color_in) {\n"
		"\treturn vec4(color_in.r * subrect.r, color_in.g * subrect.g, color_in.b * subrect.b, color_in.a);\n"
		"}\n"
		"vec4 subsprite_b(vec2 tc) {\n"
		"\treturn vec4(inner_color, texture(texture_sampler, vec2(tc.x * subrect.y + subrect.x, tc.y * subrect.a + subrect.z)).r * texture(texture_sampler, vec2(tc.x * subrect.y + subrect.x, tc.y * subrect.a + subrect.z)).a); \n"
		"}\n"
		"vec4 font_function(vec2 tc) {\n"
		"\treturn int(subroutines_index.y) == 1 ? subsprite_b(tc) : no_filter(tc);\n"
		"}\n"
		"vec4 coloring_function(vec4 tc) {\n"
		"\tswitch(int(subroutines_index.x)) {\n"
		"\tcase 3: return disabled_color(tc);\n"
		"\tcase 4: return enabled_color(tc);\n"
		"\tcase 12: return tint_color(tc);\n"
		"\tcase 13: return interactable_color(tc);\n"
		"\tcase 14: return interactable_disabled_color(tc);\n"
		"\tcase 16: return alt_tint_color(tc);\n"
		"\tdefault: break;\n"
		"\t}\n"
		"\treturn tc;\n"
		"}\n"
		"void main() {\n"
		"\tfrag_color = coloring_function(font_function(tex_coord));\n"
		"}";
	std::string_view vx_str =
		"layout (location = 0) in vec2 vertex_position;\n"
		"layout (location = 1) in vec2 v_tex_coord;\n"
		"out vec2 tex_coord;\n"
		"uniform float screen_width;\n"
		"uniform float screen_height;\n"
		"uniform vec4 d_rect;\n"
		"void main() {\n"
		"\tgl_Position = vec4(\n"
		"\t\t-1.0 + (2.0 * ((vertex_position.x * d_rect.z)  + d_rect.x) / screen_width),\n"
		"\t\t 1.0 - (2.0 * ((vertex_position.y * d_rect.w)  + d_rect.y) / screen_height),\n"
		"\t\t0.0, 1.0);\n"
		"\ttex_coord = v_tex_coord;\n"
		"}";

	ui_shader_program = create_program(vx_str, fx_str);
}

void render() {
	if(!opengl_context)
		return;

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glUseProgram(ui_shader_program);
	glUniform1i(glGetUniformLocation(ui_shader_program, "texture_sampler"), 0);
	glUniform1f(glGetUniformLocation(ui_shader_program, "screen_width"), float(base_width));
	glUniform1f(glGetUniformLocation(ui_shader_program, "screen_height"), float(base_height));
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0, 0, int32_t(base_width * scaling_factor), int32_t(base_height * scaling_factor));
	glDepthRange(-1.0f, 1.0f);

	launcher::ogl::render_textured_rect(launcher::ogl::color_modification::none, 0, 0, int32_t(base_width), int32_t(base_height), bg_tex.get_texture_handle(), ui::rotation::upright, false);

	launcher::ogl::render_new_text("Project Alice", launcher::ogl::color_modification::none, 83, 5, 26, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, launcher::ogl::fonts[1]);

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
		if(game_dir_not_found) {
			launcher::ogl::render_textured_rect(obj_under_mouse == ui_obj_create_scenario ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
				ui_rects[ui_obj_create_scenario].x,
				ui_rects[ui_obj_create_scenario].y,
				44,
				33,
				warning_tex.get_texture_handle(), ui::rotation::upright, false);
		}

		if(selected_scenario_file.empty()) {
			auto sv = launcher::localised_strings[uint8_t(launcher::string_index::create_scenario)];
			float x_pos = ui_rects[ui_obj_create_scenario].x + ui_rects[ui_obj_create_scenario].width / 2 - base_text_extent(sv.data(), uint32_t(sv.size()), 22, launcher::ogl::fonts[1]) / 2.0f;
			launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, x_pos, ui_rects[ui_obj_create_scenario].y + 2.f, 22.0f, launcher::ogl::color3f{ 50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f }, launcher::ogl::fonts[1]);
		} else {
			auto sv = launcher::localised_strings[uint8_t(launcher::string_index::recreate_scenario)];
			float x_pos = ui_rects[ui_obj_create_scenario].x + ui_rects[ui_obj_create_scenario].width / 2 - base_text_extent(sv.data(), uint32_t(sv.size()), 22, launcher::ogl::fonts[1]) / 2.0f;
			launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, x_pos, ui_rects[ui_obj_create_scenario].y + 2.f, 22.0f, launcher::ogl::color3f{ 50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f }, launcher::ogl::fonts[1]);
		}
	} else {
		launcher::ogl::render_textured_rect(launcher::ogl::color_modification::disabled,
			ui_rects[ui_obj_create_scenario].x,
			ui_rects[ui_obj_create_scenario].y,
			ui_rects[ui_obj_create_scenario].width,
			ui_rects[ui_obj_create_scenario].height,
			big_button_tex.get_texture_handle(), ui::rotation::upright, false);
		if(game_dir_not_found) {
			launcher::ogl::render_textured_rect(launcher::ogl::color_modification::disabled,
				ui_rects[ui_obj_create_scenario].x,
				ui_rects[ui_obj_create_scenario].y,
				44,
				33,
				warning_tex.get_texture_handle(), ui::rotation::upright, false);
		}
		auto sv = launcher::localised_strings[uint8_t(launcher::string_index::working)];
		float x_pos = ui_rects[ui_obj_create_scenario].x + ui_rects[ui_obj_create_scenario].width / 2 - base_text_extent(sv.data(), uint32_t(sv.size()), 22, launcher::ogl::fonts[1]) / 2.0f;
		launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, x_pos, 50.0f, 22.0f, launcher::ogl::color3f{ 50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f }, launcher::ogl::fonts[1]);
	}

	{
		// Create a new scenario file for the selected mods
		auto sv = launcher::localised_strings[uint8_t(launcher::string_index::create_a_new_scenario)];
		auto xoffset = 830.0f - base_text_extent(sv.data(), uint32_t(sv.size()), 14, launcher::ogl::fonts[0]);
		launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, xoffset, 94.0f + 0 * 18.0f, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, launcher::ogl::fonts[0]);
		sv = launcher::localised_strings[uint8_t(launcher::string_index::for_the_selected_mods)];
		xoffset = 830.0f - base_text_extent(sv.data(), uint32_t(sv.size()), 14, launcher::ogl::fonts[0]);
		launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, xoffset, 94.0f + 1 * 18.0f, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, launcher::ogl::fonts[0]);
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
			big_l_button_tex.get_texture_handle(), ui::rotation::upright, false);
		launcher::ogl::render_textured_rect(obj_under_mouse == ui_obj_join_game ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
			ui_rects[ui_obj_join_game].x,
			ui_rects[ui_obj_join_game].y,
			ui_rects[ui_obj_join_game].width,
			ui_rects[ui_obj_join_game].height,
			big_r_button_tex.get_texture_handle(), ui::rotation::upright, false);
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
			big_l_button_tex.get_texture_handle(), ui::rotation::upright, false);
		launcher::ogl::render_textured_rect(launcher::ogl::color_modification::disabled,
			ui_rects[ui_obj_join_game].x,
			ui_rects[ui_obj_join_game].y,
			ui_rects[ui_obj_join_game].width,
			ui_rects[ui_obj_join_game].height,
			big_r_button_tex.get_texture_handle(), ui::rotation::upright, false);

		/*830, 250*/
		// No scenario file found

		auto sv = launcher::localised_strings[uint8_t(launcher::string_index::no_scenario_found)];
		auto xoffset = 830.0f - base_text_extent(sv.data(), uint32_t(sv.size()), 14, launcher::ogl::fonts[0]);
		launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, xoffset, ui_rects[ui_obj_play_game].y + 48.f, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, launcher::ogl::fonts[0]);
	}

	auto sv = launcher::localised_strings[uint8_t(launcher::string_index::ip_address)];
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, ui_rects[ui_obj_ip_addr].x + ui_rects[ui_obj_ip_addr].width - base_text_extent(sv.data(), uint32_t(sv.size()), 14, launcher::ogl::fonts[0]), ui_rects[ui_obj_ip_addr].y - 21.f, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, launcher::ogl::fonts[0]);
	launcher::ogl::render_textured_rect(active_textbox == ui_obj_ip_addr ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
		ui_rects[ui_obj_ip_addr].x,
		ui_rects[ui_obj_ip_addr].y,
		ui_rects[ui_obj_ip_addr].width,
		ui_rects[ui_obj_ip_addr].height,
		line_bg_tex.get_texture_handle(), ui::rotation::upright, false);

	sv = launcher::localised_strings[uint8_t(launcher::string_index::lobby_password)];
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, ui_rects[ui_obj_password].x + ui_rects[ui_obj_password].width - base_text_extent(sv.data(), uint32_t(sv.size()), 14, launcher::ogl::fonts[0]), ui_rects[ui_obj_password].y - 21.f, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, launcher::ogl::fonts[0]);
	launcher::ogl::render_textured_rect(active_textbox == ui_obj_password ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
		ui_rects[ui_obj_password].x,
		ui_rects[ui_obj_password].y,
		ui_rects[ui_obj_password].width,
		ui_rects[ui_obj_password].height,
		line_bg_tex.get_texture_handle(), ui::rotation::upright, false);

	sv = launcher::localised_strings[uint8_t(launcher::string_index::nickname)];
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, ui_rects[ui_obj_player_name].x + ui_rects[ui_obj_player_name].width - base_text_extent(sv.data(), uint32_t(sv.size()), 14, launcher::ogl::fonts[0]), ui_rects[ui_obj_player_name].y - 21.f, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, launcher::ogl::fonts[0]);
	launcher::ogl::render_textured_rect(active_textbox == ui_obj_player_name ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
		ui_rects[ui_obj_player_name].x,
		ui_rects[ui_obj_player_name].y,
		ui_rects[ui_obj_player_name].width,
		ui_rects[ui_obj_player_name].height,
		line_bg_tex.get_texture_handle(), ui::rotation::upright, false);

	sv = launcher::localised_strings[uint8_t(launcher::string_index::player_password)];
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, ui_rects[ui_obj_player_password].x + ui_rects[ui_obj_player_password].width - base_text_extent(sv.data(), uint32_t(sv.size()), 14, launcher::ogl::fonts[0]), ui_rects[ui_obj_player_password].y - 21.f, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, launcher::ogl::fonts[0]);
	launcher::ogl::render_textured_rect(active_textbox == ui_obj_player_password ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
		ui_rects[ui_obj_player_password].x,
		ui_rects[ui_obj_player_password].y,
		ui_rects[ui_obj_player_password].width,
		ui_rects[ui_obj_player_password].height,
		line_bg_tex.get_texture_handle(), ui::rotation::upright, false);

	sv = launcher::localised_strings[uint8_t(launcher::string_index::singleplayer)];
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, ui_rects[ui_obj_play_game].x + ui_rects[ui_obj_play_game].width - base_text_extent(sv.data(), uint32_t(sv.size()), 22, launcher::ogl::fonts[1]), ui_rects[ui_obj_play_game].y - 32.f, 22.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, launcher::ogl::fonts[1]);

	sv = launcher::localised_strings[uint8_t(launcher::string_index::start_game)];
	float sg_x_pos = ui_rects[ui_obj_play_game].x + ui_rects[ui_obj_play_game].width / 2 - base_text_extent(sv.data(), uint32_t(sv.size()), 22, launcher::ogl::fonts[1]) / 2.0f;
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, sg_x_pos, ui_rects[ui_obj_play_game].y + 2.f, 22.0f, launcher::ogl::color3f{ 50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f }, launcher::ogl::fonts[1]);

	sv = launcher::localised_strings[uint8_t(launcher::string_index::multiplayer)];
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, ui_rects[ui_obj_join_game].x + ui_rects[ui_obj_join_game].width - base_text_extent(sv.data(), uint32_t(sv.size()), 22, launcher::ogl::fonts[1]), ui_rects[ui_obj_host_game].y - 32.f, 22.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, launcher::ogl::fonts[1]);

	// Join and host game buttons
	sv = launcher::localised_strings[uint8_t(launcher::string_index::host)];
	float hg_x_pos = ui_rects[ui_obj_host_game].x + ui_rects[ui_obj_host_game].width / 2 - base_text_extent(sv.data(), uint32_t(sv.size()), 22, launcher::ogl::fonts[1]) / 2.0f;
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, hg_x_pos, ui_rects[ui_obj_host_game].y + 2.f, 22.0f, launcher::ogl::color3f{ 50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f }, launcher::ogl::fonts[1]);
	sv = launcher::localised_strings[uint8_t(launcher::string_index::join)];
	float jg_x_pos = ui_rects[ui_obj_join_game].x + ui_rects[ui_obj_join_game].width / 2 - base_text_extent(sv.data(), uint32_t(sv.size()), 22, launcher::ogl::fonts[1]) / 2.0f;
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, jg_x_pos, ui_rects[ui_obj_join_game].y + 2.f, 22.0f, launcher::ogl::color3f{ 50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f }, launcher::ogl::fonts[1]);

	// Text fields
	float ia_x_pos = ui_rects[ui_obj_ip_addr].x + 6.f;// ui_rects[ui_obj_ip_addr].width - base_text_extent(ip_addr.c_str(), uint32_t(ip_addr.length()), 14, fonts[0]) - 4.f;
	launcher::ogl::render_new_text(((active_textbox == ui_obj_ip_addr && is_cursor_visible) ? (ip_addr + std::string("_")).c_str() : ip_addr.c_str()), launcher::ogl::color_modification::none, ia_x_pos, ui_rects[ui_obj_ip_addr].y + 3.f, 14.0f, launcher::ogl::color3f{ 255.0f, 255.0f, 255.0f }, launcher::ogl::fonts[0]);
	float ps_x_pos = ui_rects[ui_obj_password].x + 6.f;
	launcher::ogl::render_new_text(((active_textbox == ui_obj_password && is_cursor_visible) ? (lobby_password + std::string("_")).c_str() : lobby_password.c_str()), launcher::ogl::color_modification::none, ia_x_pos, ui_rects[ui_obj_password].y + 3.f, 14.0f, launcher::ogl::color3f{ 255.0f, 255.0f, 255.0f }, launcher::ogl::fonts[0]);
	float pn_x_pos = ui_rects[ui_obj_player_name].x + 6.f;// ui_rects[ui_obj_player_name].width - base_text_extent(player_name.c_str(), uint32_t(player_name.length()), 14, fonts[0]) - 4.f;
	launcher::ogl::render_new_text(((active_textbox == ui_obj_player_name && is_cursor_visible) ? (player_name.to_string() + std::string("_")).c_str() : player_name.to_string_view()), launcher::ogl::color_modification::none, pn_x_pos, ui_rects[ui_obj_player_name].y + 3.f, 14.0f, launcher::ogl::color3f{ 255.0f, 255.0f, 255.0f }, launcher::ogl::fonts[0]);
	float pp_x_pos = ui_rects[ui_obj_player_password].x + 6.f;// ui_rects[ui_obj_player_password].width - base_text_extent(player_name.c_str(), uint32_t(player_name.length()), 14, fonts[0]) - 4.f;
	launcher::ogl::render_new_text(((active_textbox == ui_obj_player_password && is_cursor_visible) ? (player_password.to_string() + std::string("_")).c_str() : player_password.to_string_view()), launcher::ogl::color_modification::none, pn_x_pos, ui_rects[ui_obj_player_password].y + 3.f, 14.0f, launcher::ogl::color3f{ 255.0f, 255.0f, 255.0f }, launcher::ogl::fonts[0]);

	sv = launcher::localised_strings[uint8_t(launcher::string_index::mod_list)];
	auto ml_xoffset = list_text_right_align - base_text_extent(sv.data(), uint32_t(sv.size()), 24, launcher::ogl::fonts[1]);
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, ml_xoffset, 45.0f, 24.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, launcher::ogl::fonts[1]);

	int32_t list_offset = launcher::frame_in_list * launcher::ui_list_count;

	for(int32_t i = 0; i < ui_list_count && list_offset + i < int32_t(mod_list.size()); ++i) {
		auto& mod_ref = mod_list[list_offset + i];

		if(i % 2 == 1) {
			launcher::ogl::render_textured_rect(
				launcher::ogl::color_modification::none,
				60,
				75 + ui_row_height * i,
				440,
				ui_row_height,
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

		auto xoffset = list_text_right_align - base_text_extent(mod_ref.name_.data(), uint32_t(mod_ref.name_.length()), 14, launcher::ogl::fonts[0]);

		launcher::ogl::render_new_text(mod_ref.name_.data(), launcher::ogl::color_modification::none, xoffset, 75.0f + 7.0f + i * ui_row_height, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, launcher::ogl::fonts[0]);
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
		simple_fs::add_root(fs, NATIVE("."));
		auto root = get_root(fs);

		uint8_t font_set_load = 0;
		LANGID lang = GetUserDefaultUILanguage();
		//lang = 0x0004;
		switch(lang & 0xff) {
		//case 0x0001:
		//	localised_strings = &ar_localised_strings[0];
		//	font_set_load = 2;
		//	break;
		case 0x0002:
			localised_strings = &bg_localised_strings[0];
			font_set_load = 3;
			break;
		case 0x0003:
			localised_strings = &ca_localised_strings[0];
			break;
		case 0x0004:
			localised_strings = &zh_localised_strings[0];
			font_set_load = 1;
			break;
		case 0x0005:
			localised_strings = &cs_localised_strings[0];
			break;
		case 0x0006:
			localised_strings = &da_localised_strings[0];
			break;
		case 0x0007:
			localised_strings = &de_localised_strings[0];
			break;
		case 0x0008:
			localised_strings = &el_localised_strings[0];
			break;
		case 0x0009:
			localised_strings = &en_localised_strings[0];
			break;
		case 0x000A:
			localised_strings = &es_localised_strings[0];
			break;
		case 0x000B:
			localised_strings = &fi_localised_strings[0];
			break;
		case 0x000C:
			localised_strings = &fr_localised_strings[0];
			break;
		//case 0x000D:
		//	localised_strings = &he_localised_strings[0];
		//	break;
		case 0x000E:
			localised_strings = &hu_localised_strings[0];
			break;
		case 0x000F:
			//localised_strings = &is_localised_strings[0];
			break;
		case 0x0010:
			localised_strings = &it_localised_strings[0];
			break;
		case 0x0011:
			//localised_strings = &ja_localised_strings[0];
			break;
		case 0x0012:
			//localised_strings = &ko_localised_strings[0];
			break;
		case 0x0016:
			localised_strings = &po_localised_strings[0];
			break;
		case 0x0013:
			localised_strings = &nl_localised_strings[0];
			break;
		case 0x0014:
			localised_strings = &no_localised_strings[0];
			break;
		case 0x0015:
			localised_strings = &pl_localised_strings[0];
			break;
		case 0x0018:
			localised_strings = &ro_localised_strings[0];
			break;
		case 0x0019:
			localised_strings = &ru_localised_strings[0];
			font_set_load = 3;
			break;
		case 0x001C:
			localised_strings = &sq_localised_strings[0];
			break;
		case 0x001D:
			localised_strings = &sv_localised_strings[0];
			break;
		case 0x001F:
			localised_strings = &tr_localised_strings[0];
			break;
		case 0x0022:
			localised_strings = &uk_localised_strings[0];
			font_set_load = 3;
			break;
		case 0x0025:
			localised_strings = &et_localised_strings[0];
			break;
		case 0x0026:
			localised_strings = &lv_localised_strings[0];
			break;
		case 0x0027:
			localised_strings = &lt_localised_strings[0];
			break;
		case 0x002A:
			localised_strings = &vi_localised_strings[0];
			break;
		case 0x002B:
			localised_strings = &hy_localised_strings[0];
			break;
		case 0x0039:
			localised_strings = &hi_localised_strings[0];
			break;
		default:
			break;
		}
		if(font_set_load == 0) {
			auto font_a = simple_fs::open_file(root, NATIVE("assets/fonts/LibreCaslonText-Regular.ttf"));
			if(font_a) {
				auto file_content = simple_fs::view_contents(*font_a);
				launcher::ogl::font_collection.load_font(launcher::ogl::fonts[0], file_content.data, file_content.file_size);
			}
			auto font_b = simple_fs::open_file(root, NATIVE("assets/fonts/LibreCaslonText-Italic.ttf"));
			if(font_b) {
				auto file_content = simple_fs::view_contents(*font_b);
				launcher::ogl::font_collection.load_font(launcher::ogl::fonts[1], file_content.data, file_content.file_size);
			}
		} else if(font_set_load == 1) { //chinese
			auto font_a = simple_fs::open_file(root, NATIVE("assets/fonts/STZHONGS.TTF"));
			if(font_a) {
				auto file_content = simple_fs::view_contents(*font_a);
				launcher::ogl::font_collection.load_font(launcher::ogl::fonts[0], file_content.data, file_content.file_size);
			}
			auto font_b = simple_fs::open_file(root, NATIVE("assets/fonts/STZHONGS.TTF"));
			if(font_b) {
				auto file_content = simple_fs::view_contents(*font_b);
				launcher::ogl::font_collection.load_font(launcher::ogl::fonts[1], file_content.data, file_content.file_size);
			}
		} else if(font_set_load == 2) { //arabic
			auto font_a = simple_fs::open_file(root, NATIVE("assets/fonts/NotoNaskhArabic-Bold.ttf"));
			if(font_a) {
				auto file_content = simple_fs::view_contents(*font_a);
				launcher::ogl::font_collection.load_font(launcher::ogl::fonts[0], file_content.data, file_content.file_size);
			}
			auto font_b = simple_fs::open_file(root, NATIVE("assets/fonts/NotoNaskhArabic-Regular.ttf"));
			if(font_b) {
				auto file_content = simple_fs::view_contents(*font_b);
				launcher::ogl::font_collection.load_font(launcher::ogl::fonts[1], file_content.data, file_content.file_size);
			}
		} else if(font_set_load == 3) { //cyrillic
			auto font_a = simple_fs::open_file(root, NATIVE("assets/fonts/NotoSerif-Regular.ttf"));
			if(font_a) {
				auto file_content = simple_fs::view_contents(*font_a);
				launcher::ogl::font_collection.load_font(launcher::ogl::fonts[0], file_content.data, file_content.file_size);
			}
			auto font_b = simple_fs::open_file(root, NATIVE("assets/fonts/NotoSerif-Regular.ttf"));
			if(font_b) {
				auto file_content = simple_fs::view_contents(*font_b);
				launcher::ogl::font_collection.load_font(launcher::ogl::fonts[1], file_content.data, file_content.file_size);
			}
		}

		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_bg.png"), fs, bg_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_left.png"), fs, left_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_right.png"), fs, right_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_close.png"), fs, close_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_big_button.png"), fs, big_button_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_big_left.png"), fs, big_l_button_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_big_right.png"), fs, big_r_button_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_no_check.png"), fs, empty_check_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_check.png"), fs, check_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_up.png"), fs, up_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_down.png"), fs, down_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_line_bg.png"), fs, line_bg_tex, false);
		::ogl::load_file_and_return_handle(NATIVE("assets/launcher_warning.png"), fs, warning_tex, false);

		game_dir_not_found = false;
		{
			auto f = simple_fs::peek_file(root, NATIVE("v2game.exe"));
			if(!f) {
				f = simple_fs::peek_file(root, NATIVE("victoria2.exe"));
				if(!f) {
					game_dir_not_found = true;
				}
			}
		}

		// Find all mods
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

		// Find all scenario files
		auto sdir = simple_fs::get_or_create_scenario_directory();
		auto s_files = simple_fs::list_files(sdir, NATIVE(".bin"));
		for(auto& f : s_files) {
			auto of = simple_fs::open_file(f);
			if(of) {
				auto content = view_contents(*of);
				auto desc = sys::extract_mod_information(reinterpret_cast<uint8_t const*>(content.data), content.file_size);
				if(desc.count != 0) {
					max_scenario_count = std::max(desc.count, max_scenario_count);
					scenario_files.push_back(scenario_file{ simple_fs::get_file_name(f) , desc });
				}
			}
		}

		std::sort(scenario_files.begin(), scenario_files.end(), [](scenario_file const& a, scenario_file const& b) {
			return a.ident.count > b.ident.count;
		});

		// Process command line arguments
		int num_params = 0;
		auto parsed_cmd = CommandLineToArgvW(GetCommandLineW(), &num_params);
		for(int i = 1; i < num_params; ++i) {
			if(native_string(parsed_cmd[i]) == NATIVE("-outputScenario")) {
				auto str = simple_fs::native_to_utf8(native_string(parsed_cmd[i + 1]));
				requestedScenarioFileName = str;
			}
			if(native_string(parsed_cmd[i]) == NATIVE("-modsMask")) {
				auto str = simple_fs::native_to_utf8(native_string(parsed_cmd[i + 1]));
				enabledModsMask = str;
			}
			if(native_string(parsed_cmd[i]) == NATIVE("-autoBuild")) {
				autoBuild = true;
			}
		}

		find_scenario_file();

		if(enabledModsMask != "") {
			for(auto& mod : launcher::mod_list) {
				if(mod.name_.find(enabledModsMask) != std::string::npos) {
					mod.mod_selected = true;

					recursively_add_to_list(mod);
					enforce_list_order();
					find_scenario_file();
					InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
				}
			}
			InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
		}

		if(autoBuild && file_is_ready.load(std::memory_order::memory_order_acquire)) {
			make_mod_file();
			find_scenario_file();
			InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
		}

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
			set_cursor();
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
		case WM_TIMER:
		{
			if(wParam == ID_CURSOR_TIMER) {
				is_cursor_visible = !is_cursor_visible;
				InvalidateRect((HWND)(m_hwnd), nullptr, FALSE);
			}
			return 0;
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
		case WM_KEYDOWN:
			if(GetKeyState(VK_CONTROL) & 0x8000) {
				if(wParam == L'v' || wParam == L'V') {
					if(!IsClipboardFormatAvailable(CF_TEXT))
						return 0;
					if(!OpenClipboard(m_hwnd))
						return 0;

					auto hglb = GetClipboardData(CF_TEXT);
					if(hglb != nullptr) {
						auto lptstr = GlobalLock(hglb);
						if(lptstr != nullptr) {
							std::string cb_data((char*)lptstr);
							while(cb_data.length() > 0 && isspace(cb_data.back())) {
								cb_data.pop_back();
							}
							ip_addr = cb_data;
							GlobalUnlock(hglb);
						}
					}
					CloseClipboard();
				}
			}
			return 0;
		case WM_CHAR:
		{
			if(GetKeyState(VK_CONTROL) & 0x8000) {

			} else {
				char turned_into = process_utf16_to_win1250(wchar_t(wParam));
				if(turned_into) {
					if(active_textbox == ui_obj_ip_addr) {
						if(turned_into == '\b') {
							if(!ip_addr.empty())
								ip_addr.pop_back();
						} else if(turned_into >= 32 && turned_into != '\t' && turned_into != ' ' && ip_addr.size() < 46) {
							ip_addr.push_back(turned_into);
						}
					} else if(active_textbox == ui_obj_player_name) {
						if(turned_into == '\b') {
							if(!player_name.empty()) {
								player_name.pop();
								save_playername();
							}
						} else if(turned_into >= 32 && turned_into != '\t' && turned_into != ' ') {
							player_name.append(turned_into);
							save_playername();
						}
					}
					else if (active_textbox == ui_obj_player_password) {
						if (turned_into == '\b') {
							if (!player_password.empty()) {
								player_password.pop();
								save_playerpassw();
							}
						}
						else if (turned_into >= 32 && turned_into != '\t' && turned_into != ' ') {
							player_password.append(turned_into);
							save_playerpassw();
						}
					}
					else if(active_textbox == ui_obj_password) {
						if(turned_into == '\b') {
							if(!lobby_password.empty())
								lobby_password.pop_back();
						} else if(turned_into >= 32 && turned_into != '\t' && turned_into != ' ' && lobby_password.size() < 16) {
							lobby_password.push_back(turned_into);
						}
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

	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

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
	wcex.lpszClassName = NATIVE("alice_launcher_class");

	if(RegisterClassEx(&wcex) == 0) {
		window::emit_error_message("Unable to register window class", true);
	}

	// Load from user settings
	launcher::load_playername();
	launcher::load_playerpassw();

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
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	CoUninitialize();

	return 0;
}
