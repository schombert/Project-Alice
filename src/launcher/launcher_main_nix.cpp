#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <locale>
#include <codecvt>

#include <GLFW/glfw3.h>
#include "launcher_main.hpp"

namespace launcher {
static GLFWwindow* m_window = nullptr;

double last_cursor_blink_time = 0.0;
const double CURSOR_BLINK_INTERVAL = 0.5;

void make_mod_file() {
	file_is_ready.store(false, std::memory_order_seq_cst);
	auto path = produce_mod_path();
	std::thread file_maker([path]() {
		simple_fs::file_system fs_root;
		simple_fs::restore_state(fs_root, path);
		parsers::error_handler err("");
		auto root = get_root(fs_root);
		auto common = open_directory(root, NATIVE("common"));
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
			simple_fs::restore_state(game_state->common_fs, path);
			game_state->load_scenario_data(err, bookmark_context.bookmark_dates[date_index].date_);
			if(err.fatal)
				break;
			if(date_index == 0) {
				auto sdir = simple_fs::get_or_create_scenario_directory();
				int32_t append = 0;
				auto time_stamp = uint64_t(std::time(0));
				auto base_name = to_hex(time_stamp);
				auto generated_scenario_name = base_name + NATIVE("-") + std::to_string(append) + NATIVE(".bin");
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
				sys::write_scenario_file(*game_state, std::to_string(date_index) + NATIVE(".bin"), 0);
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
				auto fname = simple_fs::get_full_name(pdir) + NATIVE("scenario_errors.txt");
				std::string command = std::string("xdg-open \"") + fname + "\"";
				system(command.c_str());
			}
		}
		file_is_ready.store(true, std::memory_order_release);

		if(autoBuild) {
			glfwSetWindowShouldClose(m_window, 1);
		}
	});

	file_maker.detach();
}

void find_scenario_file() {
	if(!file_is_ready.load(std::memory_order_acquire))
		return;

	file_is_ready.store(false, std::memory_order_seq_cst);
	selected_scenario_file = NATIVE("");
	auto mod_path = produce_mod_path();

	for(auto& f : scenario_files) {
		if(f.ident.mod_path == mod_path) {
			selected_scenario_file = f.file_name;
			break;
		}
	}
	file_is_ready.store(true, std::memory_order_release);
}

void MessageBox(const char* title, const char* message) {
	char command[1024];
	snprintf(command, sizeof(command), "zenity --info --title=\"%s\" --text=\"%s\"", title, message);
	system(command);
}

void set_cursor() {
	if(obj_under_mouse == active_textbox) {
		is_cursor_visible = true;
	} else {
		active_textbox = -1;
		is_cursor_visible = false;
	}
}

void mouse_click() {
	if(obj_under_mouse == -1) {
		return;
	}

	switch(obj_under_mouse) {
	case ui_obj_close:
		glfwSetWindowShouldClose(m_window, 1);
		return;
	case ui_obj_list_left:
		if(frame_in_list > 0)
			--frame_in_list;
		return;
	case ui_obj_list_right:
		if((frame_in_list + 1) * 14 < int32_t(mod_list.size()))
			++frame_in_list;
		return;
	case ui_obj_create_scenario:
		if(file_is_ready.load(std::memory_order_acquire))
			make_mod_file();
		return;
	case ui_obj_play_game:
		if(file_is_ready.load(std::memory_order_acquire) && !selected_scenario_file.empty()) {
			std::vector<native_string> args;
			const char* hereEnv = std::getenv("HERE");
			std::string alicePath = (hereEnv != nullptr) ? std::string(hereEnv) + "/usr/bin/Alice" : "./Alice";
			args.push_back(native_string(alicePath));
			args.push_back(selected_scenario_file);
			printf("Starting game with scenario %s\n", selected_scenario_file.c_str());

			pid_t pid = fork();
			if(pid == 0) {
				std::vector<char*> argv;
				for(const auto& s : args) {
					argv.push_back(const_cast<char*>(s.c_str()));
				}
				argv.push_back(nullptr);
				execv(argv[0], argv.data());
				perror("execv");
				exit(127);
			} else if(pid < 0) {
				perror("fork");
			}
			return;
		}
	case ui_obj_host_game:
		[[fallthrough]];
	case ui_obj_join_game:
		if(file_is_ready.load(std::memory_order_acquire) && !selected_scenario_file.empty()) {
			std::vector<std::string> args;
			args.push_back("./Alice");
			args.push_back(selected_scenario_file);

			if(obj_under_mouse == ui_obj_host_game) {
				args.push_back("-host");
				args.push_back("-name");
				args.push_back(player_name.to_string());
				if(!player_password.empty()) {
					args.push_back("-player_password");
					args.push_back(player_password.to_string());
				}
			} else if(obj_under_mouse == ui_obj_join_game) {
				args.push_back("-join");
				args.push_back(ip_addr);
				args.push_back("-name");
				args.push_back(player_name.to_string());
				if(!player_password.empty()) {
					args.push_back("-player_password");
					args.push_back(player_password.to_string());
				}
				if(!ip_addr.empty() && ip_addr.find(':') != std::string::npos) {
					args.push_back("-v6");
				}
			}

			if(!lobby_password.empty()) {
				args.push_back("-password");
				args.push_back(lobby_password);
			}

			pid_t pid = fork();
			if(pid == 0) {
				std::vector<char*> argv;
				for(const auto& s : args) {
					argv.push_back(const_cast<char*>(s.c_str()));
				}
				argv.push_back(nullptr);
				execv(argv[0], argv.data());
				perror("execv");
				exit(127);
			} else if(pid < 0) {
				perror("fork");
			}
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

	if(!file_is_ready.load(std::memory_order_acquire))
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
		}
		return;
	}
	case ui_list_move_up:
	{
		int32_t list_offset = launcher::frame_in_list * launcher::ui_list_count + list_position;
		if(launcher::mod_list[list_offset].mod_selected && nth_item_can_move_up(list_offset)) {
			std::swap(launcher::mod_list[list_offset], launcher::mod_list[list_offset - 1]);
			find_scenario_file();
		}
		return;
	}
	case ui_list_move_down:
	{
		int32_t list_offset = launcher::frame_in_list * launcher::ui_list_count + list_position;
		if(launcher::mod_list[list_offset].mod_selected && nth_item_can_move_down(list_offset)) {
			std::swap(launcher::mod_list[list_offset], launcher::mod_list[list_offset + 1]);
			find_scenario_file();
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
		MessageBox("OpenGL error", "shader creation failed");
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
		MessageBox("OpenGL error", error.c_str());
	}
	return return_value;
}

GLuint create_program(std::string_view vertex_shader, std::string_view fragment_shader) {
	GLuint return_value = glCreateProgram();
	if(return_value == 0) {
		MessageBox("OpenGL error", "program creation failed");
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
		MessageBox("OpenGL error", err.c_str());
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
		"vec4 font_function(vec2 tc) {\n"
		"\treturn int(subroutines_index.y) == 1 ? color_filter(tc) : no_filter(tc);\n"
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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if(action != GLFW_PRESS && action != GLFW_REPEAT)
		return;

	if((mods & GLFW_MOD_CONTROL) && key == GLFW_KEY_V) {
		if(active_textbox == ui_obj_ip_addr) {
			ip_addr = glfwGetClipboardString(window);
		}
		return;
	}
}

void character_callback(GLFWwindow* window, unsigned int codepoint) {
	if(active_textbox == -1)
		return;

	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
	std::string utf8_char = conv.to_bytes(codepoint);

	auto handle_text_input = [&](std::string& target, size_t max_len) {
		if(target.length() < max_len) {
			target += utf8_char;
		}
		};

	auto handle_player_name_input = [&](sys::player_name& target) {
		for(char c : utf8_char) {
			target.append(c);
		}
		};

	switch(active_textbox) {
	case ui_obj_ip_addr:
		handle_text_input(ip_addr, 46);
		break;
	case ui_obj_password:
		handle_text_input(lobby_password, 16);
		break;
	case ui_obj_player_name:
		handle_player_name_input(player_name);
		save_playername();
		break;
	case ui_obj_player_password:
		handle_player_name_input(player_password);
		save_playerpassw();
		break;
	}
}

void character_with_backspace_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if(action != GLFW_PRESS && action != GLFW_REPEAT) return;
	if(active_textbox == -1 || key != GLFW_KEY_BACKSPACE) return;

	auto handle_backspace = [&](std::string& target) {
		if(!target.empty()) {
			size_t i = target.length() - 1;
			while(i > 0 && (target[i] & 0xC0) == 0x80) {
				i--;
			}
			target.erase(i);
		}
		};

	auto handle_player_name_backspace = [&](sys::player_name& target) {
		if(!target.empty()) {
			target.pop();
		}
		};

	switch(active_textbox) {
	case ui_obj_ip_addr: handle_backspace(ip_addr); break;
	case ui_obj_password: handle_backspace(lobby_password); break;
	case ui_obj_player_name:
		handle_player_name_backspace(player_name);
		save_playername();
		break;
	case ui_obj_player_password:
		handle_player_name_backspace(player_password);
		save_playerpassw();
		break;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		mouse_click();
		set_cursor();
	}
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	mouse_x = int32_t(xpos / scaling_factor);
	mouse_y = int32_t(ypos / scaling_factor);
	update_under_mouse();
}

void window_size_callback(GLFWwindow* window, int width, int height) {
	float x_scale = (float)width / base_width;
	float y_scale = (float)height / base_height;
	scaling_factor = x_scale;
	glViewport(0, 0, width, height);
}

static void glfw_error_callback(int error, const char* description) {
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void render() {

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

	launcher::ogl::render_new_text("Project Alice", launcher::ogl::color_modification::none, 83, 5, 26, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, fonts[1]);

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

	if(file_is_ready.load(std::memory_order_acquire)) {
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
			float x_pos = ui_rects[ui_obj_create_scenario].x + ui_rects[ui_obj_create_scenario].width / 2 - base_text_extent(sv.data(), uint32_t(sv.size()), 22, fonts[1]) / 2.0f;
			launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, x_pos, ui_rects[ui_obj_create_scenario].y + 2.f, 22.0f, launcher::ogl::color3f{ 50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f }, fonts[1]);
		} else {
			auto sv = launcher::localised_strings[uint8_t(launcher::string_index::recreate_scenario)];
			float x_pos = ui_rects[ui_obj_create_scenario].x + ui_rects[ui_obj_create_scenario].width / 2 - base_text_extent(sv.data(), uint32_t(sv.size()), 22, fonts[1]) / 2.0f;
			launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, x_pos, ui_rects[ui_obj_create_scenario].y + 2.f, 22.0f, launcher::ogl::color3f{ 50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f }, fonts[1]);
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
		float x_pos = ui_rects[ui_obj_create_scenario].x + ui_rects[ui_obj_create_scenario].width / 2 - base_text_extent(sv.data(), uint32_t(sv.size()), 22, fonts[1]) / 2.0f;
		launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, x_pos, 50.0f, 22.0f, launcher::ogl::color3f{ 50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f }, fonts[1]);
	}

	{
		// Create a new scenario file for the selected mods
		auto sv = launcher::localised_strings[uint8_t(launcher::string_index::create_a_new_scenario)];
		auto xoffset = 830.0f - base_text_extent(sv.data(), uint32_t(sv.size()), 14, fonts[0]);
		launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, xoffset, 94.0f + 0 * 18.0f, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, fonts[0]);
		sv = launcher::localised_strings[uint8_t(launcher::string_index::for_the_selected_mods)];
		xoffset = 830.0f - base_text_extent(sv.data(), uint32_t(sv.size()), 14, fonts[0]);
		launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, xoffset, 94.0f + 1 * 18.0f, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, fonts[0]);
	}

	if(file_is_ready.load(std::memory_order_acquire) && !selected_scenario_file.empty()) {
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
		auto xoffset = 830.0f - base_text_extent(sv.data(), uint32_t(sv.size()), 14, fonts[0]);
		launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, xoffset, ui_rects[ui_obj_play_game].y + 48.f, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, fonts[0]);
	}

	auto sv = launcher::localised_strings[uint8_t(launcher::string_index::ip_address)];
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, ui_rects[ui_obj_ip_addr].x + ui_rects[ui_obj_ip_addr].width - base_text_extent(sv.data(), uint32_t(sv.size()), 14, fonts[0]), ui_rects[ui_obj_ip_addr].y - 21.f, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, fonts[0]);
	launcher::ogl::render_textured_rect(active_textbox == ui_obj_ip_addr ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
		ui_rects[ui_obj_ip_addr].x,
		ui_rects[ui_obj_ip_addr].y,
		ui_rects[ui_obj_ip_addr].width,
		ui_rects[ui_obj_ip_addr].height,
		line_bg_tex.get_texture_handle(), ui::rotation::upright, false);

	sv = launcher::localised_strings[uint8_t(launcher::string_index::lobby_password)];
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, ui_rects[ui_obj_password].x + ui_rects[ui_obj_password].width - base_text_extent(sv.data(), uint32_t(sv.size()), 14, fonts[0]), ui_rects[ui_obj_password].y - 21.f, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, fonts[0]);
	launcher::ogl::render_textured_rect(active_textbox == ui_obj_password ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
		ui_rects[ui_obj_password].x,
		ui_rects[ui_obj_password].y,
		ui_rects[ui_obj_password].width,
		ui_rects[ui_obj_password].height,
		line_bg_tex.get_texture_handle(), ui::rotation::upright, false);

	sv = launcher::localised_strings[uint8_t(launcher::string_index::nickname)];
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, ui_rects[ui_obj_player_name].x + ui_rects[ui_obj_player_name].width - base_text_extent(sv.data(), uint32_t(sv.size()), 14, fonts[0]), ui_rects[ui_obj_player_name].y - 21.f, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, fonts[0]);
	launcher::ogl::render_textured_rect(active_textbox == ui_obj_player_name ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
		ui_rects[ui_obj_player_name].x,
		ui_rects[ui_obj_player_name].y,
		ui_rects[ui_obj_player_name].width,
		ui_rects[ui_obj_player_name].height,
		line_bg_tex.get_texture_handle(), ui::rotation::upright, false);

	sv = launcher::localised_strings[uint8_t(launcher::string_index::player_password)];
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, ui_rects[ui_obj_player_password].x + ui_rects[ui_obj_player_password].width - base_text_extent(sv.data(), uint32_t(sv.size()), 14, fonts[0]), ui_rects[ui_obj_player_password].y - 21.f, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, fonts[0]);
	launcher::ogl::render_textured_rect(active_textbox == ui_obj_player_password ? launcher::ogl::color_modification::interactable : launcher::ogl::color_modification::none,
		ui_rects[ui_obj_player_password].x,
		ui_rects[ui_obj_player_password].y,
		ui_rects[ui_obj_player_password].width,
		ui_rects[ui_obj_player_password].height,
		line_bg_tex.get_texture_handle(), ui::rotation::upright, false);

	sv = launcher::localised_strings[uint8_t(launcher::string_index::singleplayer)];
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, ui_rects[ui_obj_play_game].x + ui_rects[ui_obj_play_game].width - base_text_extent(sv.data(), uint32_t(sv.size()), 22, fonts[1]), ui_rects[ui_obj_play_game].y - 32.f, 22.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, fonts[1]);

	sv = launcher::localised_strings[uint8_t(launcher::string_index::start_game)];
	float sg_x_pos = ui_rects[ui_obj_play_game].x + ui_rects[ui_obj_play_game].width / 2 - base_text_extent(sv.data(), uint32_t(sv.size()), 22, fonts[1]) / 2.0f;
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, sg_x_pos, ui_rects[ui_obj_play_game].y + 2.f, 22.0f, launcher::ogl::color3f{ 50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f }, fonts[1]);

	sv = launcher::localised_strings[uint8_t(launcher::string_index::multiplayer)];
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, ui_rects[ui_obj_join_game].x + ui_rects[ui_obj_join_game].width - base_text_extent(sv.data(), uint32_t(sv.size()), 22, fonts[1]), ui_rects[ui_obj_host_game].y - 32.f, 22.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, fonts[1]);

	// Join and host game buttons
	sv = launcher::localised_strings[uint8_t(launcher::string_index::host)];
	float hg_x_pos = ui_rects[ui_obj_host_game].x + ui_rects[ui_obj_host_game].width / 2 - base_text_extent(sv.data(), uint32_t(sv.size()), 22, fonts[1]) / 2.0f;
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, hg_x_pos, ui_rects[ui_obj_host_game].y + 2.f, 22.0f, launcher::ogl::color3f{ 50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f }, fonts[1]);
	sv = launcher::localised_strings[uint8_t(launcher::string_index::join)];
	float jg_x_pos = ui_rects[ui_obj_join_game].x + ui_rects[ui_obj_join_game].width / 2 - base_text_extent(sv.data(), uint32_t(sv.size()), 22, fonts[1]) / 2.0f;
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, jg_x_pos, ui_rects[ui_obj_join_game].y + 2.f, 22.0f, launcher::ogl::color3f{ 50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f }, fonts[1]);

	// Text fields
	float ia_x_pos = ui_rects[ui_obj_ip_addr].x + 6.f;// ui_rects[ui_obj_ip_addr].width - base_text_extent(ip_addr.c_str(), uint32_t(ip_addr.length()), 14, fonts[0]) - 4.f;
	launcher::ogl::render_new_text(((active_textbox == ui_obj_ip_addr && is_cursor_visible) ? (ip_addr + std::string("_")).c_str() : ip_addr.c_str()), launcher::ogl::color_modification::none, ia_x_pos, ui_rects[ui_obj_ip_addr].y + 3.f, 14.0f, launcher::ogl::color3f{ 255.0f, 255.0f, 255.0f }, fonts[0]);
	float ps_x_pos = ui_rects[ui_obj_password].x + 6.f;
	launcher::ogl::render_new_text(((active_textbox == ui_obj_password && is_cursor_visible) ? (lobby_password + std::string("_")).c_str() : lobby_password.c_str()), launcher::ogl::color_modification::none, ia_x_pos, ui_rects[ui_obj_password].y + 3.f, 14.0f, launcher::ogl::color3f{ 255.0f, 255.0f, 255.0f }, fonts[0]);
	float pn_x_pos = ui_rects[ui_obj_player_name].x + 6.f;// ui_rects[ui_obj_player_name].width - base_text_extent(player_name.c_str(), uint32_t(player_name.length()), 14, fonts[0]) - 4.f;
	launcher::ogl::render_new_text(((active_textbox == ui_obj_player_name && is_cursor_visible) ? (player_name.to_string() + std::string("_")).c_str() : player_name.to_string_view()), launcher::ogl::color_modification::none, pn_x_pos, ui_rects[ui_obj_player_name].y + 3.f, 14.0f, launcher::ogl::color3f{ 255.0f, 255.0f, 255.0f }, fonts[0]);
	float pp_x_pos = ui_rects[ui_obj_player_password].x + 6.f;// ui_rects[ui_obj_player_password].width - base_text_extent(player_name.c_str(), uint32_t(player_name.length()), 14, fonts[0]) - 4.f;
	launcher::ogl::render_new_text(((active_textbox == ui_obj_player_password && is_cursor_visible) ? (player_password.to_string() + std::string("_")).c_str() : player_password.to_string_view()), launcher::ogl::color_modification::none, pn_x_pos, ui_rects[ui_obj_player_password].y + 3.f, 14.0f, launcher::ogl::color3f{ 255.0f, 255.0f, 255.0f }, fonts[0]);

	sv = launcher::localised_strings[uint8_t(launcher::string_index::mod_list)];
	auto ml_xoffset = list_text_right_align - base_text_extent(sv.data(), uint32_t(sv.size()), 24, fonts[1]);
	launcher::ogl::render_new_text(sv.data(), launcher::ogl::color_modification::none, ml_xoffset, 45.0f, 24.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, fonts[1]);

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

		auto xoffset = list_text_right_align - base_text_extent(mod_ref.name_.data(), uint32_t(mod_ref.name_.length()), 14, fonts[0]);

		launcher::ogl::render_new_text(mod_ref.name_.data(), launcher::ogl::color_modification::none, xoffset, 75.0f + 7.0f + i * ui_row_height, 14.0f, launcher::ogl::color3f{ 255.0f / 255.0f, 230.0f / 255.0f, 153.0f / 255.0f }, fonts[0]);
	}

}

}

int main(int argc, char* argv[]) {

	using namespace launcher;

	glfwSetErrorCallback(glfw_error_callback);

	if(!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_DECORATED, GL_FALSE);
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

	m_window = glfwCreateWindow(base_width, base_height, "Project Alice Launcher", NULL, NULL);
	if(!m_window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1);

	if(glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	glfwSetWindowContentScaleCallback(m_window, [](GLFWwindow* window, float xscale, float yscale) {
		scaling_factor = xscale;
		glViewport(0, 0, int(base_width * scaling_factor), int(base_height * scaling_factor));
	});

	float xscale, yscale;
	glfwGetWindowContentScale(m_window, &xscale, &yscale);
	scaling_factor = xscale;

	glfwSetFramebufferSizeCallback(m_window, window_size_callback);
	glfwSetKeyCallback(m_window, key_callback);
	glfwSetKeyCallback(m_window, character_with_backspace_callback);
	glfwSetCharCallback(m_window, character_callback);
	glfwSetCursorPosCallback(m_window, cursor_position_callback);
	glfwSetMouseButtonCallback(m_window, mouse_button_callback);

	glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
	glEnable(GL_LINE_SMOOTH);

	load_shaders();
	load_global_squares();

	simple_fs::file_system fs;
	simple_fs::add_root(fs, NATIVE("."));
	auto root = get_root(fs);

	uint8_t font_set_load = 0;

	std::string locale_name(getenv("LANG"));
	size_t separator_pos = locale_name.find_first_of(".");
	std::string language = locale_name.substr(0, separator_pos);

	if(language == "en_US" || language == "en_GB" || language == "en") {
		localised_strings = &en_localised_strings[0];
	} else if(language == "zh_CN" || language == "zh_TW" || language == "zh_HK" || language == "zh") {
		localised_strings = &zh_localised_strings[0];
		font_set_load = 1;
	} else if(language == "ar_SA" || language == "ar_EG" || language == "ar") {
		localised_strings = &ar_localised_strings[0];
		font_set_load = 2;
	} else if(language == "ru_RU" || language == "ru") {
		localised_strings = &ru_localised_strings[0];
		font_set_load = 3;
	} else if(language == "uk_UA" || language == "uk") {
		localised_strings = &uk_localised_strings[0];
		font_set_load = 3;
	} else if(language == "hy_AM" || language == "hy") {
		localised_strings = &hy_localised_strings[0];
	} else if(language == "he_IL" || language == "he") {
		localised_strings = &he_localised_strings[0];
	} else if(language == "hi_IN" || language == "hi") {
		localised_strings = &hi_localised_strings[0];
	} else if(language == "vi_VN" || language == "vi") {
		localised_strings = &vi_localised_strings[0];
	} else if(language == "el_GR" || language == "el") {
		localised_strings = &el_localised_strings[0];
	} else if(language == "bg_BG" || language == "bg") {
		localised_strings = &bg_localised_strings[0];
		font_set_load = 3;
	} else if(language == "hu_HU" || language == "hu") {
		localised_strings = &hu_localised_strings[0];
	} else if(language == "pl_PL" || language == "pl") {
		localised_strings = &pl_localised_strings[0];
	} else if(language == "ro_RO" || language == "ro") {
		localised_strings = &ro_localised_strings[0];
	} else if(language == "lt_LT" || language == "lt") {
		localised_strings = &lt_localised_strings[0];
	} else if(language == "lv_LV" || language == "lv") {
		localised_strings = &lv_localised_strings[0];
	} else if(language == "et_EE" || language == "et") {
		localised_strings = &et_localised_strings[0];
	} else if(language == "sq_AL" || language == "sq") {
		localised_strings = &sq_localised_strings[0];
	} else if(language == "ca_ES" || language == "ca") {
		localised_strings = &ca_localised_strings[0];
	} else if(language == "cs_CZ" || language == "cs") {
		localised_strings = &cs_localised_strings[0];
	} else if(language == "da_DK" || language == "da") {
		localised_strings = &da_localised_strings[0];
	} else if(language == "fi_FI" || language == "fi") {
		localised_strings = &fi_localised_strings[0];
	} else if(language == "sv_SE" || language == "sv") {
		localised_strings = &sv_localised_strings[0];
	} else if(language == "no_NO" || language == "nb_NO" || language == "nn_NO" || language == "no") {
		localised_strings = &no_localised_strings[0];
	} else if(language == "de_DE" || language == "de") {
		localised_strings = &de_localised_strings[0];
	} else if(language == "fr_FR" || language == "fr") {
		localised_strings = &fr_localised_strings[0];
	} else if(language == "es_ES" || language == "es") {
		localised_strings = &es_localised_strings[0];
	} else if(language == "it_IT" || language == "it") {
		localised_strings = &it_localised_strings[0];
	} else if(language == "pt_PT" || language == "pt_BR" || language == "pt") {
		localised_strings = &po_localised_strings[0];
	} else if(language == "tr_TR" || language == "tr") {
		localised_strings = &tr_localised_strings[0];
	} else if(language == "nl_NL" || language == "nl") {
		localised_strings = &nl_localised_strings[0];
	} else {
		localised_strings = &en_localised_strings[0];
	}

	if(font_set_load == 0) {
		auto font_a = simple_fs::open_file(root, NATIVE("assets/fonts/LibreCaslonText-Regular.ttf"));
		if(font_a) {
			auto file_content = simple_fs::view_contents(*font_a);
			font_collection.load_font(fonts[0], file_content.data, file_content.file_size);
		}
		auto font_b = simple_fs::open_file(root, NATIVE("assets/fonts/LibreCaslonText-Italic.ttf"));
		if(font_b) {
			auto file_content = simple_fs::view_contents(*font_b);
			font_collection.load_font(fonts[1], file_content.data, file_content.file_size);
		}
	} else if(font_set_load == 1) { //chinese
		auto font_a = simple_fs::open_file(root, NATIVE("assets/fonts/STZHONGS.TTF"));
		if(font_a) {
			auto file_content = simple_fs::view_contents(*font_a);
			font_collection.load_font(fonts[0], file_content.data, file_content.file_size);
		}
		auto font_b = simple_fs::open_file(root, NATIVE("assets/fonts/STZHONGS.TTF"));
		if(font_b) {
			auto file_content = simple_fs::view_contents(*font_b);
			font_collection.load_font(fonts[1], file_content.data, file_content.file_size);
		}
	} else if(font_set_load == 2) { //arabic
		auto font_a = simple_fs::open_file(root, NATIVE("assets/fonts/NotoNaskhArabic-Bold.ttf"));
		if(font_a) {
			auto file_content = simple_fs::view_contents(*font_a);
			font_collection.load_font(fonts[0], file_content.data, file_content.file_size);
		}
		auto font_b = simple_fs::open_file(root, NATIVE("assets/fonts/NotoNaskhArabic-Regular.ttf"));
		if(font_b) {
			auto file_content = simple_fs::view_contents(*font_b);
			font_collection.load_font(fonts[1], file_content.data, file_content.file_size);
		}
	} else if(font_set_load == 3) { //cyrillic
		auto font_a = simple_fs::open_file(root, NATIVE("assets/fonts/NotoSerif-Regular.ttf"));
		if(font_a) {
			auto file_content = simple_fs::view_contents(*font_a);
			font_collection.load_font(fonts[0], file_content.data, file_content.file_size);
		}
		auto font_b = simple_fs::open_file(root, NATIVE("assets/fonts/NotoSerif-Regular.ttf"));
		if(font_b) {
			auto file_content = simple_fs::view_contents(*font_b);
			font_collection.load_font(fonts[1], file_content.data, file_content.file_size);
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

	for(int i = 1; i < argc; ++i) {
		std::string arg = argv[i];
		if(arg == "-outputScenario" && i + 1 < argc) {
			requestedScenarioFileName = argv[++i];
		} else if(arg == "-modsMask" && i + 1 < argc) {
			enabledModsMask = argv[++i];
		} else if(arg == "-autoBuild") {
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
			}
		}
	}

	if(autoBuild && file_is_ready.load(std::memory_order_acquire)) {
		make_mod_file();
		find_scenario_file();
	}

	find_scenario_file();

	load_playername();
	load_playerpassw();

	while(!glfwWindowShouldClose(m_window)) {
		if(active_textbox != -1 && (glfwGetTime() - last_cursor_blink_time > CURSOR_BLINK_INTERVAL)) {
			is_cursor_visible = !is_cursor_visible;
			last_cursor_blink_time = glfwGetTime();
		}

		render();
		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}

	glfwDestroyWindow(m_window);
	glfwTerminate();

	return 0;
}
