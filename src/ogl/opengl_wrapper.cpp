#include "opengl_wrapper.hpp"
#include "system_state.hpp"
#include "simple_fs.hpp"

namespace ogl {

std::string_view opengl_get_error_name(GLenum t) {
	switch(t) {
		case GL_INVALID_ENUM:
			return "GL_INVALID_ENUM";
		case GL_INVALID_VALUE:
			return "GL_INVALID_VALUE";
		case GL_INVALID_OPERATION:
			return "GL_INVALID_OPERATION";
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			return "GL_INVALID_FRAMEBUFFER_OPERATION";
		case GL_OUT_OF_MEMORY:
			return "GL_OUT_OF_MEMORY";
		case GL_STACK_UNDERFLOW:
			return "GL_STACK_UNDERFLOW";
		case GL_STACK_OVERFLOW:
			return "GL_STACK_OVERFLOW";
		case GL_NO_ERROR:
			return "GL_NO_ERROR";
		default:
			return "Unknown";
	}
}

void notify_user_of_fatal_opengl_error(std::string message) {
	std::string full_message = message;
	full_message += "\n";
	full_message += opengl_get_error_name(glGetError());
#ifdef _WIN64
	MessageBoxA(nullptr, full_message.c_str(), "OpenGL error", MB_OK);
#else
	std::fprintf(stderr, "OpenGL error: %s\n", full_message.c_str());
#endif
	std::abort();
}


GLint compile_shader(std::string_view source, GLenum type) {
	GLuint return_value = glCreateShader(type);

	if(return_value == 0) {
		notify_user_of_fatal_opengl_error("shader creation failed");
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
		notify_user_of_fatal_opengl_error(std::string("Shader failed to compile:\n") + log.get());
	}
	return return_value;
}

GLuint create_program(std::string_view vertex_shader, std::string_view fragment_shader) {
	GLuint return_value = glCreateProgram();
	if(return_value == 0) {
		notify_user_of_fatal_opengl_error("program creation failed");
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
		notify_user_of_fatal_opengl_error(std::string("Program failed to link:\n") + log);
	}

	glDeleteShader(v_shader);
	glDeleteShader(f_shader);

	return return_value;
}

void load_special_icons(sys::state& state) {
	auto root = get_root(state.common_fs);
	auto gfx_dir = simple_fs::open_directory(root, NATIVE("gfx"));

	auto interface_dir = simple_fs::open_directory(gfx_dir, NATIVE("interface"));
	auto money_dds = simple_fs::open_file(interface_dir, NATIVE("icon_money_big.dds"));
	if(money_dds) {
		auto content = simple_fs::view_contents(*money_dds);
		uint32_t size_x, size_y;
		state.open_gl.money_icon_tex = GLuint(ogl::SOIL_direct_load_DDS_from_memory(reinterpret_cast<uint8_t const*>(content.data),
				content.file_size, size_x, size_y, 0));
	}

	auto assets_dir = simple_fs::open_directory(root, NATIVE("assets"));
	auto cross_dds = simple_fs::open_file(assets_dir, NATIVE("trigger_not.dds"));
	if(cross_dds) {
		auto content = simple_fs::view_contents(*cross_dds);
		uint32_t size_x, size_y;
		state.open_gl.cross_icon_tex = GLuint(ogl::SOIL_direct_load_DDS_from_memory(reinterpret_cast<uint8_t const*>(content.data),
				content.file_size, size_x, size_y, 0));
	}
	auto checkmark_dds = simple_fs::open_file(assets_dir, NATIVE("trigger_yes.dds"));
	if(checkmark_dds) {
		auto content = simple_fs::view_contents(*checkmark_dds);
		uint32_t size_x, size_y;
		state.open_gl.checkmark_icon_tex = GLuint(ogl::SOIL_direct_load_DDS_from_memory(
				reinterpret_cast<uint8_t const*>(content.data), content.file_size, size_x, size_y, 0));
	}

	auto n_dds = simple_fs::open_file(interface_dir, NATIVE("politics_foreign_naval_units.dds"));
	if(n_dds) {
		auto content = simple_fs::view_contents(*n_dds);
		uint32_t size_x, size_y;
		state.open_gl.navy_icon_tex = GLuint(ogl::SOIL_direct_load_DDS_from_memory(
			reinterpret_cast<uint8_t const*>(content.data), content.file_size, size_x, size_y, 0));
	}
	auto a_dds = simple_fs::open_file(interface_dir, NATIVE("topbar_army.dds"));
	if(a_dds) {
		auto content = simple_fs::view_contents(*a_dds);
		uint32_t size_x, size_y;
		state.open_gl.army_icon_tex = GLuint(ogl::SOIL_direct_load_DDS_from_memory(
			reinterpret_cast<uint8_t const*>(content.data), content.file_size, size_x, size_y, 0));
	}
}

std::string_view framebuffer_error(GLenum e) {
	switch(e) {
	case GL_FRAMEBUFFER_UNDEFINED:
		return "GL_FRAMEBUFFER_UNDEFINED";
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT ";
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
	case GL_FRAMEBUFFER_UNSUPPORTED:
		return "GL_FRAMEBUFFER_UNSUPPORTED";
	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
		return "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
	default:
		break;
	}
	return "???";
}

void initialize_msaa(sys::state& state, int32_t size_x, int32_t size_y) {
	if(state.user_settings.antialias_level == 0)
		return;
	if(!size_x || !size_y)
		return;
	glEnable(GL_MULTISAMPLE);
	// setup screen VAO
	static const float sq_vertices[] = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};
	glGenVertexArrays(1, &state.open_gl.msaa_vao);
	glGenBuffers(1, &state.open_gl.msaa_vbo);
	glBindVertexArray(state.open_gl.msaa_vao);
	glBindBuffer(GL_ARRAY_BUFFER, state.open_gl.msaa_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sq_vertices), &sq_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	// framebuffer
	glGenFramebuffers(1, &state.open_gl.msaa_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, state.open_gl.msaa_framebuffer);
	// create a multisampled color attachment texture
	glGenTextures(1, &state.open_gl.msaa_texcolorbuffer);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, state.open_gl.msaa_texcolorbuffer);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, GLsizei(state.user_settings.antialias_level), GL_RGBA, size_x, size_y, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, state.open_gl.msaa_texcolorbuffer, 0);
	// create a (also multisampled) renderbuffer object for depth and stencil attachments
	glGenRenderbuffers(1, &state.open_gl.msaa_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, state.open_gl.msaa_rbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, GLsizei(state.user_settings.antialias_level), GL_DEPTH24_STENCIL8, size_x, size_y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, state.open_gl.msaa_rbo);
	if(auto r = glCheckFramebufferStatus(GL_FRAMEBUFFER); r != GL_FRAMEBUFFER_COMPLETE) {
		notify_user_of_fatal_opengl_error("MSAA framebuffer wasn't completed: " + std::string(framebuffer_error(r)));
	}
	// configure second post-processing framebuffer
	glGenFramebuffers(1, &state.open_gl.msaa_interbuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, state.open_gl.msaa_interbuffer);
	// create a color attachment texture
	glGenTextures(1, &state.open_gl.msaa_texture);
	glBindTexture(GL_TEXTURE_2D, state.open_gl.msaa_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size_x, size_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, state.open_gl.msaa_texture, 0);	// we only need a color buffer
	if(auto r = glCheckFramebufferStatus(GL_FRAMEBUFFER); r != GL_FRAMEBUFFER_COMPLETE) {
		notify_user_of_fatal_opengl_error("MSAA post processing framebuffer wasn't completed: " + std::string(framebuffer_error(r)));
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	auto root = get_root(state.common_fs);
	auto msaa_fshader = open_file(root, NATIVE("assets/shaders/msaa_f_shader.glsl"));
	auto msaa_vshader = open_file(root, NATIVE("assets/shaders/msaa_v_shader.glsl"));
	if(bool(msaa_fshader) && bool(msaa_vshader)) {
		auto vertex_content = view_contents(*msaa_vshader);
		auto fragment_content = view_contents(*msaa_fshader);
		state.open_gl.msaa_shader_program = create_program(std::string_view(vertex_content.data, vertex_content.file_size),
				std::string_view(fragment_content.data, fragment_content.file_size));
	} else {
		notify_user_of_fatal_opengl_error("Unable to open a MSAA shaders file");
	}
	state.open_gl.msaa_enabled = true;
}


void deinitialize_msaa(sys::state& state) {
	if(!state.open_gl.msaa_enabled)
		return;

	state.open_gl.msaa_enabled = false;
	if(state.open_gl.msaa_texture)
		glDeleteTextures(1, &state.open_gl.msaa_texture);
	if(state.open_gl.msaa_interbuffer)
		glDeleteFramebuffers(1, &state.open_gl.msaa_framebuffer);
	if(state.open_gl.msaa_rbo)
		glDeleteRenderbuffers(1, &state.open_gl.msaa_rbo);
	if(state.open_gl.msaa_texcolorbuffer)
		glDeleteTextures(1, &state.open_gl.msaa_texcolorbuffer);
	if(state.open_gl.msaa_framebuffer)
		glDeleteFramebuffers(1, &state.open_gl.msaa_framebuffer);
	if(state.open_gl.msaa_vbo)
		glDeleteBuffers(1, &state.open_gl.msaa_vbo);
	if(state.open_gl.msaa_vao)
		glDeleteVertexArrays(1, &state.open_gl.msaa_vao);
	if(state.open_gl.msaa_shader_program)
		glDeleteProgram(state.open_gl.msaa_shader_program);
	glDisable(GL_MULTISAMPLE);
}

void initialize_opengl(sys::state& state) {
	create_opengl_context(state);

	glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
	glEnable(GL_LINE_SMOOTH);

	load_shaders(state);				// create shaders
	load_global_squares(state); // create various squares to drive the shaders with

	state.flag_type_map.resize(size_t(culture::flag_type::count), 0);
	// Create the remapping for flags
	state.world.for_each_national_identity([&](dcon::national_identity_id ident_id) {
		auto fat_id = dcon::fatten(state.world, ident_id);
		auto nat_id = fat_id.get_nation_from_identity_holder().id;
		for(auto gov_id : state.world.in_government_type) {
			state.flag_types.push_back(culture::flag_type(gov_id.get_flag()));
		}
	});
	// Eliminate duplicates
	std::sort(state.flag_types.begin(), state.flag_types.end());
	state.flag_types.erase(std::unique(state.flag_types.begin(), state.flag_types.end()), state.flag_types.end());

	// Automatically assign texture offsets to the flag_types
	auto id = 0;
	for(auto type : state.flag_types)
		state.flag_type_map[uint32_t(type)] = uint8_t(id++);
	assert(state.flag_type_map[0] == 0); // default_flag

	// Allocate textures for the flags
	state.open_gl.asset_textures.resize(
			state.ui_defs.textures.size() + (state.world.national_identity_size() + 1) * state.flag_types.size());

	state.map_state.load_map(state);

	load_special_icons(state);
	state.font_collection.load_all_glyphs();

	initialize_msaa(state, window::creation_parameters().size_x, window::creation_parameters().size_y);
}

static GLfloat global_square_data[] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.0f};

static GLfloat global_square_right_data[] = {0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f};

static GLfloat global_square_left_data[] = {0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		1.0f, 1.0f};

static GLfloat global_square_flipped_data[] = {0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f};

static GLfloat global_square_right_flipped_data[] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f};

static GLfloat global_square_left_flipped_data[] = {0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f};

void load_shaders(sys::state& state) {
	auto root = get_root(state.common_fs);
	auto ui_fshader = open_file(root, NATIVE("assets/shaders/ui_f_shader.glsl"));
	auto ui_vshader = open_file(root, NATIVE("assets/shaders/ui_v_shader.glsl"));
	if(bool(ui_fshader) && bool(ui_vshader)) {
		auto vertex_content = view_contents(*ui_vshader);
		auto fragment_content = view_contents(*ui_fshader);
		state.open_gl.ui_shader_program = create_program(std::string_view(vertex_content.data, vertex_content.file_size),
				std::string_view(fragment_content.data, fragment_content.file_size));
	} else {
		notify_user_of_fatal_opengl_error("Unable to open a necessary shader file");
	}
}

void load_global_squares(sys::state& state) {
	glGenBuffers(1, &state.open_gl.global_square_buffer);

	// Populate the position buffer
	glBindBuffer(GL_ARRAY_BUFFER, state.open_gl.global_square_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_square_data, GL_STATIC_DRAW);

	glGenVertexArrays(1, &state.open_gl.global_square_vao);
	glBindVertexArray(state.open_gl.global_square_vao);
	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // texture coordinates

	glBindVertexBuffer(0, state.open_gl.global_square_buffer, 0, sizeof(GLfloat) * 4);

	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, 0);									 // position
	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2); // texture coordinates
	glVertexAttribBinding(0, 0);																				 // position -> to array zero
	glVertexAttribBinding(1, 0);																				 // texture coordinates -> to array zero

	glGenBuffers(1, &state.open_gl.global_square_left_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, state.open_gl.global_square_left_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_square_left_data, GL_STATIC_DRAW);

	glGenBuffers(1, &state.open_gl.global_square_right_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, state.open_gl.global_square_right_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_square_right_data, GL_STATIC_DRAW);

	glGenBuffers(1, &state.open_gl.global_square_right_flipped_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, state.open_gl.global_square_right_flipped_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_square_right_flipped_data, GL_STATIC_DRAW);

	glGenBuffers(1, &state.open_gl.global_square_left_flipped_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, state.open_gl.global_square_left_flipped_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_square_left_flipped_data, GL_STATIC_DRAW);

	glGenBuffers(1, &state.open_gl.global_square_flipped_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, state.open_gl.global_square_flipped_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_square_flipped_data, GL_STATIC_DRAW);

	glGenBuffers(64, state.open_gl.sub_square_buffers);
	for(uint32_t i = 0; i < 64; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, state.open_gl.sub_square_buffers[i]);

		float const cell_x = static_cast<float>(i & 7) / 8.0f;
		float const cell_y = static_cast<float>((i >> 3) & 7) / 8.0f;

		GLfloat global_sub_square_data[] = {0.0f, 0.0f, cell_x, cell_y, 0.0f, 1.0f, cell_x, cell_y + 1.0f / 8.0f, 1.0f, 1.0f,
				cell_x + 1.0f / 8.0f, cell_y + 1.0f / 8.0f, 1.0f, 0.0f, cell_x + 1.0f / 8.0f, cell_y};

		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_sub_square_data, GL_STATIC_DRAW);
	}
}

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

void bind_vertices_by_rotation(sys::state const& state, ui::rotation r, bool flipped) {
	switch(r) {
	case ui::rotation::upright:
		if(!flipped)
			glBindVertexBuffer(0, state.open_gl.global_square_buffer, 0, sizeof(GLfloat) * 4);
		else
			glBindVertexBuffer(0, state.open_gl.global_square_flipped_buffer, 0, sizeof(GLfloat) * 4);
		break;
	case ui::rotation::r90_left:
		if(!flipped)
			glBindVertexBuffer(0, state.open_gl.global_square_left_buffer, 0, sizeof(GLfloat) * 4);
		else
			glBindVertexBuffer(0, state.open_gl.global_square_left_flipped_buffer, 0, sizeof(GLfloat) * 4);
		break;
	case ui::rotation::r90_right:
		if(!flipped)
			glBindVertexBuffer(0, state.open_gl.global_square_right_buffer, 0, sizeof(GLfloat) * 4);
		else
			glBindVertexBuffer(0, state.open_gl.global_square_right_flipped_buffer, 0, sizeof(GLfloat) * 4);
		break;
	}
}

void render_textured_rect(sys::state const& state, color_modification enabled, float x, float y, float width, float height,
		GLuint texture_handle, ui::rotation r, bool flipped) {
	glBindVertexArray(state.open_gl.global_square_vao);

	bind_vertices_by_rotation(state, r, flipped);

	glUniform4f(parameters::drawing_rectangle, x, y, width, height);
	// glUniform4f(parameters::drawing_rectangle, 0, 0, width, height);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_handle);

	GLuint subroutines[2] = {map_color_modification_to_index(enabled), parameters::no_filter};
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_textured_rect_direct(sys::state const& state, float x, float y, float width, float height, uint32_t handle) {
	glBindVertexArray(state.open_gl.global_square_vao);

	glBindVertexBuffer(0, state.open_gl.global_square_buffer, 0, sizeof(GLfloat) * 4);

	glUniform4f(parameters::drawing_rectangle, x, y, width, height);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, handle);

	GLuint subroutines[2] = {parameters::enabled, parameters::no_filter};
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_linegraph(sys::state const& state, color_modification enabled, float x, float y, float width, float height,
		lines& l) {
	glBindVertexArray(state.open_gl.global_square_vao);

	l.bind_buffer();

	glUniform4f(parameters::drawing_rectangle, x, y, width, height);
	glLineWidth(2.0f);

	GLuint subroutines[2] = {map_color_modification_to_index(enabled), parameters::linegraph};
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(l.count));
}

void render_barchart(sys::state const& state, color_modification enabled, float x, float y, float width, float height,
		data_texture& t, ui::rotation r, bool flipped) {
	glBindVertexArray(state.open_gl.global_square_vao);

	bind_vertices_by_rotation(state, r, flipped);

	glUniform4f(parameters::drawing_rectangle, x, y, width, height);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, t.handle());

	GLuint subroutines[2] = {map_color_modification_to_index(enabled), parameters::barchart};
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_piechart(sys::state const& state, color_modification enabled, float x, float y, float size, data_texture& t) {
	glBindVertexArray(state.open_gl.global_square_vao);

	glBindVertexBuffer(0, state.open_gl.global_square_buffer, 0, sizeof(GLfloat) * 4);

	glUniform4f(parameters::drawing_rectangle, x, y, size, size);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, t.handle());

	GLuint subroutines[2] = {map_color_modification_to_index(enabled), parameters::piechart};
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_bordered_rect(sys::state const& state, color_modification enabled, float border_size, float x, float y, float width,
		float height, GLuint texture_handle, ui::rotation r, bool flipped) {
	glBindVertexArray(state.open_gl.global_square_vao);

	bind_vertices_by_rotation(state, r, flipped);

	glUniform4f(parameters::drawing_rectangle, x, y, width, height);
	glUniform1f(parameters::border_size, border_size);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_handle);

	GLuint subroutines[2] = {map_color_modification_to_index(enabled), parameters::frame_stretch};
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_masked_rect(sys::state const& state, color_modification enabled, float x, float y, float width, float height,
		GLuint texture_handle, GLuint mask_texture_handle, ui::rotation r, bool flipped) {
	glBindVertexArray(state.open_gl.global_square_vao);

	bind_vertices_by_rotation(state, r, flipped);

	glUniform4f(parameters::drawing_rectangle, x, y, width, height);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_handle);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mask_texture_handle);

	GLuint subroutines[2] = {map_color_modification_to_index(enabled), parameters::use_mask};
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_progress_bar(sys::state const& state, color_modification enabled, float progress, float x, float y, float width,
		float height, GLuint left_texture_handle, GLuint right_texture_handle, ui::rotation r, bool flipped) {
	glBindVertexArray(state.open_gl.global_square_vao);

	bind_vertices_by_rotation(state, r, flipped);

	glUniform4f(parameters::drawing_rectangle, x, y, width, height);
	glUniform1f(parameters::border_size, progress);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, left_texture_handle);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, right_texture_handle);

	GLuint subroutines[2] = {map_color_modification_to_index(enabled), parameters::progress_bar};
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_tinted_textured_rect(sys::state const& state, float x, float y, float width, float height, float r, float g, float b,
		GLuint texture_handle, ui::rotation rot, bool flipped) {
	glBindVertexArray(state.open_gl.global_square_vao);

	bind_vertices_by_rotation(state, rot, flipped);

	glUniform3f(parameters::inner_color, r, g, b);
	glUniform4f(parameters::drawing_rectangle, x, y, width, height);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_handle);

	GLuint subroutines[2] = {parameters::tint, parameters::no_filter};
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_tinted_subsprite(sys::state const& state, int frame, int total_frames, float x, float y,
		float width, float height, float r, float g, float b, GLuint texture_handle, ui::rotation rot, bool flipped) {
	glBindVertexArray(state.open_gl.global_square_vao);

	bind_vertices_by_rotation(state, rot, flipped);

	auto const scale = 1.0f / static_cast<float>(total_frames);
	glUniform3f(parameters::inner_color, static_cast<float>(frame) * scale, scale, 0.0f);
	glUniform4f(parameters::subrect, r, g, b, 0);
	glUniform4f(parameters::drawing_rectangle, x, y, width, height);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_handle);

	GLuint subroutines[2] = { parameters::alternate_tint, parameters::sub_sprite };
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_subsprite(sys::state const& state, color_modification enabled, int frame, int total_frames, float x, float y,
		float width, float height, GLuint texture_handle, ui::rotation r, bool flipped) {
	glBindVertexArray(state.open_gl.global_square_vao);

	bind_vertices_by_rotation(state, r, flipped);

	auto const scale = 1.0f / static_cast<float>(total_frames);
	glUniform3f(parameters::inner_color, static_cast<float>(frame) * scale, scale, 0.0f);
	glUniform4f(parameters::drawing_rectangle, x, y, width, height);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_handle);

	GLuint subroutines[2] = {map_color_modification_to_index(enabled), parameters::sub_sprite};
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_character(sys::state const& state, char codepoint, color_modification enabled, float x, float y, float size, text::font& f) {
	if(text::win1250toUTF16(codepoint) != ' ') {
		// f.make_glyph(codepoint);

		glBindVertexBuffer(0, state.open_gl.sub_square_buffers[uint8_t(codepoint) & 63], 0, sizeof(GLfloat) * 4);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, f.textures[uint8_t(codepoint) >> 6]);

		glUniform4f(parameters::drawing_rectangle, x, y, size, size);
		glUniform3f(parameters::inner_color, 0.0f, 0.0f, 0.0f);
		glUniform1f(parameters::border_size, 0.06f * 16.0f / size);

		GLuint subroutines[2] = {map_color_modification_to_index(enabled), parameters::border_filter};
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}
}

GLuint get_flag_texture_handle_from_tag(sys::state& state, char tag[3]) {
	tag[0] = char(toupper(tag[0]));
	tag[1] = char(toupper(tag[1]));
	tag[2] = char(toupper(tag[2]));

	dcon::national_identity_id ident{};
	state.world.for_each_national_identity([&](dcon::national_identity_id id) {
		auto curr = nations::int_to_tag(state.world.national_identity_get_identifying_int(id));
		if(curr[0] == tag[0] && curr[1] == tag[1] && curr[2] == tag[2]) {
			ident = id;
		}
	});
	if(!bool(ident)) {
		// QOL: We will print the text instead of displaying the flag, for ease of viewing invalid tags
		return 0;
	}
	auto fat_id = dcon::fatten(state.world, ident);
	auto nation = fat_id.get_nation_from_identity_holder();
	culture::flag_type flag_type = culture::flag_type{};
	if(bool(nation.id) && nation.get_owned_province_count() != 0) {
		flag_type = culture::get_current_flag_type(state, nation.id);
	} else {
		flag_type = culture::get_current_flag_type(state, ident);
	}
	return ogl::get_flag_handle(state, ident, flag_type);
}

bool display_tag_is_valid(sys::state& state, char tag[3]) {
	tag[0] = char(toupper(tag[0]));
	tag[1] = char(toupper(tag[1]));
	tag[2] = char(toupper(tag[2]));
	dcon::national_identity_id ident{};
	state.world.for_each_national_identity([&](dcon::national_identity_id id) {
		auto curr = nations::int_to_tag(state.world.national_identity_get_identifying_int(id));
		if(curr[0] == tag[0] && curr[1] == tag[1] && curr[2] == tag[2])
			ident = id;
	});
	return bool(ident);
}

void internal_text_render(sys::state& state, char const* codepoints, uint32_t count, float x, float baseline_y, float size,
		text::font& f, GLuint const* subroutines, GLuint const* icon_subroutines) {
	for(uint32_t i = 0; i < count; ++i) {
		if(text::win1250toUTF16(codepoints[i]) != ' ') {
			// f.make_glyph(codepoints[i]);
			if(text::win1250toUTF16(codepoints[i]) == u'\u0040') {
				char tag[3] = { 0, 0, 0 };
				tag[0] = (i + 1 < count) ? char(codepoints[i + 1]) : 0;
				tag[1] = (i + 2 < count) ? char(codepoints[i + 2]) : 0;
				tag[2] = (i + 3 < count) ? char(codepoints[i + 3]) : 0;
				GLuint flag_texture_handle = get_flag_texture_handle_from_tag(state, tag);
				if(flag_texture_handle != 0) {
					bind_vertices_by_rotation(state, ui::rotation::upright, false);
					glActiveTexture(GL_TEXTURE0);
					glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, icon_subroutines);
					glBindTexture(GL_TEXTURE_2D, flag_texture_handle);
					glUniform4f(parameters::drawing_rectangle, x,
							baseline_y + f.glyph_positions[0x4D].y * size / 64.0f, size * 1.5f, size);
					glUniform4f(ogl::parameters::subrect, 0.f /* x offset */, 1.f /* x width */, 0.f /* y offset */, 1.f /* y height */
					);
					glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
					glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines);

					x += size * 1.5f;
					
					i += 3;
					continue;
				}
			}  // fallthrough on purpose: if it doesn't match a flag, render it as text

			if(text::win1250toUTF16(codepoints[i]) == u'\u0001' || text::win1250toUTF16(codepoints[i]) == u'\u0002') {
				bind_vertices_by_rotation(state, ui::rotation::upright, false);
				glActiveTexture(GL_TEXTURE0);
				glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, icon_subroutines);
				glBindTexture(GL_TEXTURE_2D, text::win1250toUTF16(codepoints[i]) == u'\u0001' ? state.open_gl.cross_icon_tex : state.open_gl.checkmark_icon_tex);
				glUniform4f(parameters::drawing_rectangle, x, baseline_y + f.glyph_positions[0x4D].y * size / 64.0f, size, size);
				glUniform4f(ogl::parameters::subrect, 0.f /* x offset */, 1.f /* x width */, 0.f /* y offset */, 1.f /* y height */
				);
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
				glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines);

				x += size;
			} else if(text::win1250toUTF16(codepoints[i]) == u'\u0003' || text::win1250toUTF16(codepoints[i]) == u'\u0004') {
				bind_vertices_by_rotation(state, ui::rotation::upright, false);
				glActiveTexture(GL_TEXTURE0);
				glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, icon_subroutines);
				glBindTexture(GL_TEXTURE_2D, text::win1250toUTF16(codepoints[i]) == u'\u0003' ? state.open_gl.army_icon_tex : state.open_gl.navy_icon_tex);
				glUniform4f(parameters::drawing_rectangle, x - size * 0.125f, baseline_y - size * 0.25f + f.glyph_positions[0x4D].y * size / 64.0f, size * 1.5f, size * 1.5f);
				glUniform4f(ogl::parameters::subrect, 0.f /* x offset */, 1.f /* x width */, 0.f /* y offset */, 1.f /* y height */
				);
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
				glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines);

				x += size;
			} else {
				glBindVertexBuffer(0, state.open_gl.sub_square_buffers[uint8_t(codepoints[i]) & 63], 0, sizeof(GLfloat) * 4);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, f.textures[uint8_t(codepoints[i]) >> 6]);
				glUniform4f(parameters::drawing_rectangle, x + f.glyph_positions[uint8_t(codepoints[i])].x * size / 64.0f,
						baseline_y + f.glyph_positions[uint8_t(codepoints[i])].y * size / 64.0f, size, size);
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

				x += f.glyph_advances[uint8_t(codepoints[i])] * size / 64.0f +
						 ((i != count - 1) ? f.kerning(codepoints[i], codepoints[i + 1]) * size / 64.0f : 0.0f);
			}
		} else {
			x += f.glyph_advances[uint8_t(codepoints[i])] * size / 64.0f +
					 ((i != count - 1) ? f.kerning(codepoints[i], codepoints[i + 1]) * size / 64.0f : 0.0f);
		}
	}
}

void render_new_text(sys::state& state, char const* codepoints, uint32_t count, color_modification enabled, float x,
		float y, float size, color3f const& c, text::font& f) {
	glUniform3f(parameters::inner_color, c.r, c.g, c.b);
	glUniform1f(parameters::border_size, 0.08f * 16.0f / size);

	GLuint subroutines[2] = {map_color_modification_to_index(enabled), parameters::filter};
	GLuint icon_subroutines[2] = {map_color_modification_to_index(enabled), parameters::no_filter};
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines);
	internal_text_render(state, codepoints, count, x, y + size, size, f, subroutines, icon_subroutines);
}

void render_classic_text(sys::state& state, float x, float y, char const* codepoints, uint32_t count,
		color_modification enabled, color3f const& c, text::bm_font const& font) {
	float adv = 1.0f / font.width; // Font texture atlas spacing.

	bind_vertices_by_rotation(state, ui::rotation::upright, false);

	GLuint subroutines[2] = {map_color_modification_to_index(enabled), parameters::subsprite_b};
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines);

	// Set Text Color, all one color for now.

	//------ FOR SCHOMBERT ------//
	// Every iteration of this loop draws one character of the string 'fmt'.
	//'texlst' contains information for each vertex of each rectangle for each character.
	// Every 4 elements in 'texlst' is one complete rectangle, and one character.
	//'texlst[i].texx' and 'texlst[i].texy' are the intended texture coordinates of a vertex on the texture.
	//'texlst[i].x' and 'texlst[i].y' are the coordinates of a vertex of the rendered rectangle in the window.
	// The color variables are unused currently.
	//
	// Spacing, kearning, etc. are already applied.
	// Scaling (unintentionally) is also applied (by whatever part of Alice scales the normal fonts).

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, font.ftexid);

	for(uint32_t i = 0; i < count; ++i) {
		auto f = font.chars[0];
		if(uint8_t(codepoints[i]) == 0x40) {
			char tag[3] = { 0, 0, 0 };
			tag[0] = (i + 1 < count) ? char(codepoints[i + 1]) : 0;
			tag[1] = (i + 2 < count) ? char(codepoints[i + 2]) : 0;
			tag[2] = (i + 3 < count) ? char(codepoints[i + 3]) : 0;
			GLuint flag_texture_handle = get_flag_texture_handle_from_tag(state, tag);
			if(flag_texture_handle != 0) {
				GLuint flag_subroutines[2] = {map_color_modification_to_index(enabled), parameters::no_filter};
				glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, flag_subroutines);
				f = font.chars[0x4D];
				float scaling = uint8_t(codepoints[i]) == 0xA4 ? 1.5f : 1.f;
				float offset = uint8_t(codepoints[i]) == 0xA4 ? 0.25f : 0.f;
				float CurX = x + f.x_offset - (float(f.width) * offset);
				float CurY = y + f.y_offset - (float(f.height) * offset);
				glUniform4f(ogl::parameters::drawing_rectangle, CurX, CurY, float(f.height) * 1.5f * scaling, float(f.height) * scaling);
				glBindTexture(GL_TEXTURE_2D, flag_texture_handle);
				glUniform3f(parameters::inner_color, c.r, c.g, c.b);
				glUniform4f(ogl::parameters::subrect, float(f.x) / float(font.width) /* x offset */,
						float(f.width) / float(font.width) /* x width */, float(f.y) / float(font.width) /* y offset */,
						float(f.height) / float(font.width) /* y height */
				);
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
				// Restore affected state
				glBindTexture(GL_TEXTURE_2D, font.ftexid);
				glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines);

				x += f.x_offset - (float(f.width) * offset) + float(f.height) * 1.5f * scaling;

				i += 3;
				continue;
			}
		}

		if(uint8_t(codepoints[i]) == 0xA4 || uint8_t(codepoints[i]) == 0x01 || uint8_t(codepoints[i]) == 0x02 || int8_t(codepoints[i]) == 0x03 || uint8_t(codepoints[i]) == 0x04) {
			GLuint money_subroutines[2] = {map_color_modification_to_index(enabled), parameters::no_filter};
			glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, money_subroutines);
			f = font.chars[0x4D];
			float scaling = uint8_t(codepoints[i]) == 0xA4 ? 1.5f : 1.f;
			float offset = uint8_t(codepoints[i]) == 0xA4 ? 0.25f : 0.f;
			float CurX = x + f.x_offset - (float(f.width) * offset);
			float CurY = y + f.y_offset - (float(f.height) * offset);
			glUniform4f(ogl::parameters::drawing_rectangle, CurX, CurY, float(f.width) * scaling, float(f.height) * scaling);
			glBindTexture(GL_TEXTURE_2D, uint8_t(codepoints[i]) == 0xA4		? state.open_gl.money_icon_tex
																	 : uint8_t(codepoints[i]) == 0x01 ? state.open_gl.cross_icon_tex
																																		: state.open_gl.checkmark_icon_tex);
			glUniform3f(parameters::inner_color, c.r, c.g, c.b);
			glUniform4f(ogl::parameters::subrect, float(f.x) / float(font.width) /* x offset */,
					float(f.width) / float(font.width) /* x width */, float(f.y) / float(font.width) /* y offset */,
					float(f.height) / float(font.width) /* y height */
			);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			// Restore affected state
			glBindTexture(GL_TEXTURE_2D, font.ftexid);
			glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines);

			x += f.x_offset - (float(f.width) * offset) + float(f.width) * scaling;
			continue;
		} else {
			f = font.chars[uint8_t(codepoints[i])];
			float CurX = x + f.x_offset;
			float CurY = y + f.y_offset;
			glUniform4f(ogl::parameters::drawing_rectangle, CurX, CurY, float(f.width), float(f.height));
			glUniform3f(parameters::inner_color, c.r, c.g, c.b);
			glUniform4f(ogl::parameters::subrect, float(f.x) / float(font.width) /* x offset */,
					float(f.width) / float(font.width) /* x width */, float(f.y) / float(font.width) /* y offset */,
					float(f.height) / float(font.width) /* y height */
			);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		// Only check kerning if there is greater then 1 character and
		// if the check character is 1 less then the end of the string.
		if(i != count - 1) {
			x += font.get_kerning_pair(codepoints[i], codepoints[i + 1]);
		}
		x += f.x_advance;
	}
}

void render_text(sys::state& state, char const* codepoints, uint32_t count, color_modification enabled, float x, float y,
		color3f const& c, uint16_t font_id) {
	if(state.user_settings.use_classic_fonts) {
		render_classic_text(state, x, y, codepoints, count, enabled, c, text::get_bm_font(state, font_id));
	} else {
		render_new_text(state, codepoints, count, enabled, x, y, float(text::size_from_font_id(font_id)), c,
				state.font_collection.fonts[text::font_index_from_font_id(font_id) - 1]);
	}
}

void lines::set_y(float* v) {
	for(int32_t i = 0; i < static_cast<int32_t>(count); ++i) {
		buffer[i * 4] = static_cast<float>(i) / static_cast<float>(count - 1);
		buffer[i * 4 + 1] = 1.0f - v[i];
		buffer[i * 4 + 2] = 0.5f;
		buffer[i * 4 + 3] = v[i];
	}
	pending_data_update = true;
}

void lines::set_default_y() {
	for(int32_t i = 0; i < static_cast<int32_t>(count); ++i) {
		buffer[i * 4] = static_cast<float>(i) / static_cast<float>(count - 1);
		buffer[i * 4 + 1] = 0.5f;
		buffer[i * 4 + 2] = 0.5f;
		buffer[i * 4 + 3] = 0.5f;
	}
	pending_data_update = true;
}

void lines::bind_buffer() {
	if(buffer_handle == 0) {
		glGenBuffers(1, &buffer_handle);

		glBindBuffer(GL_ARRAY_BUFFER, buffer_handle);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * count * 4, nullptr, GL_DYNAMIC_DRAW);
	}
	if(buffer && pending_data_update) {
		glBindBuffer(GL_ARRAY_BUFFER, buffer_handle);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * count * 4, buffer);
		pending_data_update = false;
	}

	glBindVertexBuffer(0, buffer_handle, 0, sizeof(GLfloat) * 4);
}

bool msaa_enabled(sys::state const& state) {
	return state.open_gl.msaa_enabled;
}

} // namespace ogl
