#include "opengl_wrapper.hpp"
#include "system_state.hpp"
#include "simple_fs.hpp"
#include "fonts.hpp"
#include "bmfont.hpp"
#include "gui_element_base.hpp"

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
	window::emit_error_message("OpenGL error:" + full_message, true);
}


GLint compile_shader(std::string_view source, GLenum type) {
	GLuint return_value = glCreateShader(type);

	if(return_value == 0) {
		notify_user_of_fatal_opengl_error("shader creation failed");
	}

	std::string s_source(source);
	GLchar const* texts[] = {
		"#version 140\r\n",
		"#extension GL_ARB_explicit_uniform_location : enable\r\n",
		"#extension GL_ARB_explicit_attrib_location : enable\r\n",
		"#extension GL_ARB_shader_subroutine : enable\r\n",
		"#extension GL_ARB_vertex_array_object : enable\r\n"
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
				content.file_size, size_x, size_y, ogl::SOIL_FLAG_TEXTURE_REPEATS));
	}

	auto assets_dir = simple_fs::open_directory(root, NATIVE("assets"));
	auto cross_dds = simple_fs::open_file(assets_dir, NATIVE("trigger_not.dds"));
	if(cross_dds) {
		auto content = simple_fs::view_contents(*cross_dds);
		uint32_t size_x, size_y;
		state.open_gl.cross_icon_tex = GLuint(ogl::SOIL_direct_load_DDS_from_memory(reinterpret_cast<uint8_t const*>(content.data),
				content.file_size, size_x, size_y, ogl::SOIL_FLAG_TEXTURE_REPEATS));
	}
	auto cb_cross_dds = simple_fs::open_file(assets_dir, NATIVE("trigger_not_cb.dds"));
	if(cb_cross_dds) {
		auto content = simple_fs::view_contents(*cb_cross_dds);
		uint32_t size_x, size_y;
		state.open_gl.color_blind_cross_icon_tex = GLuint(ogl::SOIL_direct_load_DDS_from_memory(reinterpret_cast<uint8_t const*>(content.data),
			content.file_size, size_x, size_y, ogl::SOIL_FLAG_TEXTURE_REPEATS));
	}
	auto checkmark_dds = simple_fs::open_file(assets_dir, NATIVE("trigger_yes.dds"));
	if(checkmark_dds) {
		auto content = simple_fs::view_contents(*checkmark_dds);
		uint32_t size_x, size_y;
		state.open_gl.checkmark_icon_tex = GLuint(ogl::SOIL_direct_load_DDS_from_memory(
				reinterpret_cast<uint8_t const*>(content.data), content.file_size, size_x, size_y, ogl::SOIL_FLAG_TEXTURE_REPEATS));
	}

	auto n_dds = simple_fs::open_file(interface_dir, NATIVE("politics_foreign_naval_units.dds"));
	if(n_dds) {
		auto content = simple_fs::view_contents(*n_dds);
		uint32_t size_x, size_y;
		state.open_gl.navy_icon_tex = GLuint(ogl::SOIL_direct_load_DDS_from_memory(
			reinterpret_cast<uint8_t const*>(content.data), content.file_size, size_x, size_y, ogl::SOIL_FLAG_TEXTURE_REPEATS));
	}
	auto a_dds = simple_fs::open_file(interface_dir, NATIVE("topbar_army.dds"));
	if(a_dds) {
		auto content = simple_fs::view_contents(*a_dds);
		uint32_t size_x, size_y;
		state.open_gl.army_icon_tex = GLuint(ogl::SOIL_direct_load_DDS_from_memory(
			reinterpret_cast<uint8_t const*>(content.data), content.file_size, size_x, size_y, ogl::SOIL_FLAG_TEXTURE_REPEATS));
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

void initialize_framebuffer_for_province_indices(sys::state& state, int32_t size_x, int32_t size_y) {
	// prepare textures for rendering
	glGenTextures(1, &state.open_gl.province_map_rendertexture);
	glBindTexture(GL_TEXTURE_2D, state.open_gl.province_map_rendertexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, size_x, size_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenRenderbuffers(1, &state.open_gl.province_map_depthbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, state.open_gl.province_map_depthbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size_x, size_y);

	// framebuffer
	glGenFramebuffers(1, &state.open_gl.province_map_framebuffer);
	state.console_log(ogl::opengl_get_error_name(glGetError()));
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, state.open_gl.province_map_framebuffer);
	state.console_log(ogl::opengl_get_error_name(glGetError()));
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, state.open_gl.province_map_rendertexture, 0);
	state.console_log(ogl::opengl_get_error_name(glGetError()));
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, state.open_gl.province_map_depthbuffer);
	state.console_log(ogl::opengl_get_error_name(glGetError()));

	// drawbuffers
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);

	auto check = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

	if(check != GL_FRAMEBUFFER_COMPLETE) {
		state.console_log(ogl::framebuffer_error(check));
	} else {
		state.console_log("provincial framebuffer is ready");
	}
}

void deinitialize_framebuffer_for_province_indices(sys::state& state) {
	if(state.open_gl.province_map_rendertexture)
		glDeleteTextures(1, &state.open_gl.province_map_rendertexture);
	if(state.open_gl.province_map_rendertexture)
		glDeleteRenderbuffers(1, &state.open_gl.province_map_rendertexture);
	if(state.open_gl.province_map_framebuffer)
		glDeleteFramebuffers(1, &state.open_gl.province_map_framebuffer);
	
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
		//notify_user_of_fatal_opengl_error("MSAA framebuffer wasn't completed: " + std::string(framebuffer_error(r)));
		state.user_settings.antialias_level--;
		deinitialize_msaa(state);
		if(state.user_settings.antialias_level != 0) {
			initialize_msaa(state, size_x, size_y);
		} else {
			state.open_gl.msaa_enabled = false;
		}
		return;
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
	auto msaa_fshader = open_file(root, NATIVE("assets/shaders/glsl/msaa_f_shader.glsl"));
	auto msaa_vshader = open_file(root, NATIVE("assets/shaders/glsl/msaa_v_shader.glsl"));
	if(bool(msaa_fshader) && bool(msaa_vshader)) {
		auto vertex_content = view_contents(*msaa_vshader);
		auto fragment_content = view_contents(*msaa_fshader);
		state.open_gl.msaa_shader_program = create_program(std::string_view(vertex_content.data, vertex_content.file_size), std::string_view(fragment_content.data, fragment_content.file_size));
		state.open_gl.msaa_uniform_screen_size = glGetUniformLocation(state.open_gl.msaa_shader_program, "screen_size");
		state.open_gl.msaa_uniform_gaussian_blur = glGetUniformLocation(state.open_gl.msaa_shader_program, "gaussian_radius");
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

	load_shaders(state); // create shaders
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

	initialize_msaa(state, window::creation_parameters().size_x, window::creation_parameters().size_y);

	initialize_framebuffer_for_province_indices(state, window::creation_parameters().size_x, window::creation_parameters().size_y);
}

static const GLfloat global_square_data[] = {
	0.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 1.0f, 0.0f
};
static const GLfloat global_square_right_data[] = {
	0.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 0.0f
};
static const GLfloat global_square_left_data[] = {
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f, 1.0f
};
static const GLfloat global_square_flipped_data[] = {
	0.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 1.0f, 1.0f
};
static const GLfloat global_square_right_flipped_data[] = {
	0.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f
};
static const GLfloat global_square_left_flipped_data[] = {
	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 1.0f, 0.0f
};

//RTL squares
static const GLfloat global_rtl_square_data[] = {
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 0.0f
};
static const GLfloat global_rtl_square_right_data[] = {
	0.0f, 1.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f
};
static const GLfloat global_rtl_square_left_data[] = {
	0.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f
};
static const GLfloat global_rtl_square_flipped_data[] = {
	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 1.0f
};
static const GLfloat global_rtl_square_right_flipped_data[] = {
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f, 1.0f
};
static const GLfloat global_rtl_square_left_flipped_data[] = {
	0.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 0.0f
};

void load_shaders(sys::state& state) {
	auto root = get_root(state.common_fs);
	auto ui_fshader = open_file(root, NATIVE("assets/shaders/glsl/ui_f_shader.glsl"));
	auto ui_vshader = open_file(root, NATIVE("assets/shaders/glsl/ui_v_shader.glsl"));
	if(bool(ui_fshader) && bool(ui_vshader)) {
		auto vertex_content = view_contents(*ui_vshader);
		auto fragment_content = view_contents(*ui_fshader);
		state.open_gl.ui_shader_program = create_program(std::string_view(vertex_content.data, vertex_content.file_size), std::string_view(fragment_content.data, fragment_content.file_size));

		state.open_gl.ui_shader_texture_sampler_uniform = glGetUniformLocation(state.open_gl.ui_shader_program, "texture_sampler");
		state.open_gl.ui_shader_secondary_texture_sampler_uniform = glGetUniformLocation(state.open_gl.ui_shader_program, "secondary_texture_sampler");
		state.open_gl.ui_shader_screen_width_uniform = glGetUniformLocation(state.open_gl.ui_shader_program, "screen_width");
		state.open_gl.ui_shader_screen_height_uniform = glGetUniformLocation(state.open_gl.ui_shader_program, "screen_height");
		state.open_gl.ui_shader_gamma_uniform = glGetUniformLocation(state.open_gl.ui_shader_program, "gamma");

		state.open_gl.ui_shader_d_rect_uniform = glGetUniformLocation(state.open_gl.ui_shader_program, "d_rect");
		state.open_gl.ui_shader_subroutines_index_uniform = glGetUniformLocation(state.open_gl.ui_shader_program, "subroutines_index");
		state.open_gl.ui_shader_inner_color_uniform = glGetUniformLocation(state.open_gl.ui_shader_program, "inner_color");
		state.open_gl.ui_shader_subrect_uniform = glGetUniformLocation(state.open_gl.ui_shader_program, "subrect");
		state.open_gl.ui_shader_border_size_uniform = glGetUniformLocation(state.open_gl.ui_shader_program, "border_size");
	} else {
		notify_user_of_fatal_opengl_error("Unable to open a necessary shader file");
	}
}

void load_global_squares(sys::state& state) {
	// Populate the position buffer
	glGenBuffers(1, &state.open_gl.global_square_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, state.open_gl.global_square_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_square_data, GL_STATIC_DRAW);
	//RTL version
	glGenBuffers(1, &state.open_gl.global_rtl_square_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, state.open_gl.global_rtl_square_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_rtl_square_data, GL_STATIC_DRAW);

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

	//RTL mode squares
	glGenBuffers(1, &state.open_gl.global_rtl_square_left_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, state.open_gl.global_rtl_square_left_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_rtl_square_left_data, GL_STATIC_DRAW);

	glGenBuffers(1, &state.open_gl.global_rtl_square_right_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, state.open_gl.global_rtl_square_right_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_rtl_square_right_data, GL_STATIC_DRAW);

	glGenBuffers(1, &state.open_gl.global_rtl_square_right_flipped_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, state.open_gl.global_rtl_square_right_flipped_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_rtl_square_right_flipped_data, GL_STATIC_DRAW);

	glGenBuffers(1, &state.open_gl.global_rtl_square_left_flipped_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, state.open_gl.global_rtl_square_left_flipped_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_rtl_square_left_flipped_data, GL_STATIC_DRAW);

	glGenBuffers(1, &state.open_gl.global_rtl_square_flipped_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, state.open_gl.global_rtl_square_flipped_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_rtl_square_flipped_data, GL_STATIC_DRAW);

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

void bind_vertices_by_rotation(sys::state const& state, ui::rotation r, bool flipped, bool rtl) {
	switch(r) {
	case ui::rotation::upright:
		if(!flipped)
			glBindVertexBuffer(0, rtl ? state.open_gl.global_rtl_square_buffer : state.open_gl.global_square_buffer, 0, sizeof(GLfloat) * 4);
		else
			glBindVertexBuffer(0, rtl ? state.open_gl.global_rtl_square_flipped_buffer : state.open_gl.global_square_flipped_buffer, 0, sizeof(GLfloat) * 4);
		break;
	case ui::rotation::r90_left:
		if(!flipped)
			glBindVertexBuffer(0, rtl ? state.open_gl.global_rtl_square_left_buffer: state.open_gl.global_square_left_buffer, 0, sizeof(GLfloat) * 4);
		else
			glBindVertexBuffer(0, rtl ? state.open_gl.global_rtl_square_left_flipped_buffer : state.open_gl.global_square_left_flipped_buffer, 0, sizeof(GLfloat) * 4);
		break;
	case ui::rotation::r90_right:
		if(!flipped)
			glBindVertexBuffer(0, rtl ? state.open_gl.global_rtl_square_right_buffer : state.open_gl.global_square_right_buffer, 0, sizeof(GLfloat) * 4);
		else
			glBindVertexBuffer(0, rtl ? state.open_gl.global_rtl_square_right_flipped_buffer : state.open_gl.global_square_right_flipped_buffer, 0, sizeof(GLfloat) * 4);
		break;
	}
}

void render_colored_rect(
	sys::state const& state,
	float x, float y, float width, float height,
	float red, float green, float blue,
	ui::rotation r, bool flipped, bool rtl
) {
	glBindVertexArray(state.open_gl.global_square_vao);
	bind_vertices_by_rotation(state, r, flipped, rtl);
	glUniform4f(state.open_gl.ui_shader_d_rect_uniform, x, y, width, height);
	GLuint subroutines[2] = { map_color_modification_to_index(color_modification::none), parameters::solid_color };
	glUniform2ui(state.open_gl.ui_shader_subroutines_index_uniform, subroutines[0], subroutines[1]);
	glUniform3f(state.open_gl.ui_shader_inner_color_uniform, red, green, blue);
	//glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call
	glLineWidth(2.0f);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_alpha_colored_rect(
	sys::state const& state,
	float x, float y, float width, float height,
	float red, float green, float blue, float alpha
) {
	glBindVertexArray(state.open_gl.global_square_vao);
	glBindVertexBuffer(0, state.open_gl.global_square_buffer, 0, sizeof(GLfloat) * 4);
	glUniform4f(state.open_gl.ui_shader_d_rect_uniform, x, y, width, height);
	GLuint subroutines[2] = { map_color_modification_to_index(color_modification::none), parameters::alpha_color };
	glUniform2ui(state.open_gl.ui_shader_subroutines_index_uniform, subroutines[0], subroutines[1]);
	glUniform3f(state.open_gl.ui_shader_inner_color_uniform, red, green, blue);
	glUniform1f(state.open_gl.ui_shader_border_size_uniform, alpha);
	//glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call
	glLineWidth(2.0f);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_simple_rect(sys::state const& state, float x, float y, float width, float height, ui::rotation r, bool flipped, bool rtl) {
	render_colored_rect(
		state, x, y, width, height, 1.0f, 1.0f, 1.0f, r, flipped, rtl
	);
}

void render_textured_rect(sys::state const& state, color_modification enabled, float x, float y, float width, float height,
		GLuint texture_handle, ui::rotation r, bool flipped, bool rtl) {
	glBindVertexArray(state.open_gl.global_square_vao);

	bind_vertices_by_rotation(state, r, flipped, rtl);

	glUniform4f(state.open_gl.ui_shader_d_rect_uniform, x, y, width, height);
	// glUniform4f(state.open_gl.ui_shader_d_rect_uniform, 0, 0, width, height);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_handle);

	GLuint subroutines[2] = {map_color_modification_to_index(enabled), parameters::no_filter};
	glUniform2ui(state.open_gl.ui_shader_subroutines_index_uniform, subroutines[0], subroutines[1]);
	//glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_textured_rect_direct(sys::state const& state, float x, float y, float width, float height, uint32_t handle) {
	glBindVertexArray(state.open_gl.global_square_vao);

	glBindVertexBuffer(0, state.open_gl.global_square_buffer, 0, sizeof(GLfloat) * 4);

	glUniform4f(state.open_gl.ui_shader_d_rect_uniform, x, y, width, height);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, handle);

	GLuint subroutines[2] = {parameters::enabled, parameters::no_filter};
	glUniform2ui(state.open_gl.ui_shader_subroutines_index_uniform, subroutines[0], subroutines[1]);
	//glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_linegraph(sys::state const& state, color_modification enabled, float x, float y, float width, float height,
		lines& l) {
	glBindVertexArray(state.open_gl.global_square_vao);

	l.bind_buffer();

	glUniform4f(state.open_gl.ui_shader_d_rect_uniform, x, y, width, height);
	GLuint subroutines[2] = { map_color_modification_to_index(enabled), parameters::linegraph };
	glUniform2ui(state.open_gl.ui_shader_subroutines_index_uniform, subroutines[0], subroutines[1]);
	//glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	if(state.user_settings.color_blind_mode != sys::color_blind_mode::none
	&& state.user_settings.color_blind_mode != sys::color_blind_mode::achroma) {
		glLineWidth(4.0f);
		glUniform3f(state.open_gl.ui_shader_inner_color_uniform, 0.f, 0.f, 0.f);
		glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(l.count));
	}
	glLineWidth(2.0f);
	if(state.user_settings.color_blind_mode == sys::color_blind_mode::achroma) {
		glUniform3f(state.open_gl.ui_shader_inner_color_uniform, 0.f, 0.f, 0.f);
	} else if(state.user_settings.color_blind_mode == sys::color_blind_mode::tritan) {
		glUniform3f(state.open_gl.ui_shader_inner_color_uniform, 1.f, 1.f, 0.f);
	} else if(state.user_settings.color_blind_mode == sys::color_blind_mode::deutan || state.user_settings.color_blind_mode == sys::color_blind_mode::protan) {
		glUniform3f(state.open_gl.ui_shader_inner_color_uniform, 1.f, 1.f, 1.f);
	} else {
		glUniform3f(state.open_gl.ui_shader_inner_color_uniform, 1.f, 1.f, 0.f);
	}
	glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(l.count));
}

void render_linegraph(sys::state const& state, color_modification enabled, float x, float y, float width, float height, float r, float g, float b,
		lines& l) {
	glBindVertexArray(state.open_gl.global_square_vao);

	l.bind_buffer();

	glUniform4f(state.open_gl.ui_shader_d_rect_uniform, x, y, width, height);
	GLuint subroutines[2] = { map_color_modification_to_index(enabled), parameters::linegraph_color };
	glUniform2ui(state.open_gl.ui_shader_subroutines_index_uniform, subroutines[0], subroutines[1]);
	//glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	if(state.user_settings.color_blind_mode != sys::color_blind_mode::none
	&& state.user_settings.color_blind_mode != sys::color_blind_mode::achroma) {
		glLineWidth(4.0f);
		glUniform3f(state.open_gl.ui_shader_inner_color_uniform, 0.f, 0.f, 0.f);
		glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(l.count));
	}
	glLineWidth(2.0f);
	glUniform3f(state.open_gl.ui_shader_inner_color_uniform, r, g, b);
	glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(l.count));
}

void render_linegraph(sys::state const& state, color_modification enabled, float x, float y, float width, float height, float r, float g, float b, float a, lines& l) {
	glBindVertexArray(state.open_gl.global_square_vao);

	l.bind_buffer();

	glUniform4f(state.open_gl.ui_shader_d_rect_uniform, x, y, width, height);
	GLuint subroutines[2] = { map_color_modification_to_index(enabled), parameters::linegraph_acolor };
	glUniform2ui(state.open_gl.ui_shader_subroutines_index_uniform, subroutines[0], subroutines[1]);
	//glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glUniform1f(state.open_gl.ui_shader_border_size_uniform, a);

	if(state.user_settings.color_blind_mode != sys::color_blind_mode::none
	&& state.user_settings.color_blind_mode != sys::color_blind_mode::achroma) {
		glLineWidth(2.0f + 2.0f * state.user_settings.ui_scale);
		glUniform3f(state.open_gl.ui_shader_inner_color_uniform, 0.f, 0.f, 0.f);
		glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(l.count));
	}
	glLineWidth(2.0f * state.user_settings.ui_scale);
	glUniform3f(state.open_gl.ui_shader_inner_color_uniform, r, g, b);
	glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(l.count));
}

void render_barchart(sys::state const& state, color_modification enabled, float x, float y, float width, float height,
		data_texture& t, ui::rotation r, bool flipped, bool rtl) {
	glBindVertexArray(state.open_gl.global_square_vao);

	bind_vertices_by_rotation(state, r, flipped, rtl);

	glUniform4f(state.open_gl.ui_shader_d_rect_uniform, x, y, width, height);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, t.handle());

	GLuint subroutines[2] = {map_color_modification_to_index(enabled), parameters::barchart};
	glUniform2ui(state.open_gl.ui_shader_subroutines_index_uniform, subroutines[0], subroutines[1]);
	//glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_piechart(sys::state const& state, color_modification enabled, float x, float y, float size, data_texture& t) {
	glBindVertexArray(state.open_gl.global_square_vao);

	glBindVertexBuffer(0, state.open_gl.global_square_buffer, 0, sizeof(GLfloat) * 4);

	glUniform4f(state.open_gl.ui_shader_d_rect_uniform, x, y, size, size);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, t.handle());

	GLuint subroutines[2] = {map_color_modification_to_index(enabled), parameters::piechart};
	glUniform2ui(state.open_gl.ui_shader_subroutines_index_uniform, subroutines[0], subroutines[1]);
	//glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
void render_stripchart(sys::state const& state, color_modification enabled, float x, float y, float sizex, float sizey, data_texture& t) {
	glBindVertexArray(state.open_gl.global_square_vao);

	glBindVertexBuffer(0, state.open_gl.global_square_buffer, 0, sizeof(GLfloat) * 4);

	glUniform4f(state.open_gl.ui_shader_d_rect_uniform, x, y, sizex, sizey);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, t.handle());

	GLuint subroutines[2] = { map_color_modification_to_index(enabled), parameters::stripchart };
	glUniform2ui(state.open_gl.ui_shader_subroutines_index_uniform, subroutines[0], subroutines[1]);
	//glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
void render_bordered_rect(sys::state const& state, color_modification enabled, float border_size, float x, float y, float width,
		float height, GLuint texture_handle, ui::rotation r, bool flipped, bool rtl) {
	glBindVertexArray(state.open_gl.global_square_vao);

	bind_vertices_by_rotation(state, r, flipped, rtl);

	glUniform4f(state.open_gl.ui_shader_d_rect_uniform, x, y, width, height);
	glUniform1f(state.open_gl.ui_shader_border_size_uniform, border_size);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_handle);

	GLuint subroutines[2] = {map_color_modification_to_index(enabled), parameters::frame_stretch};
	glUniform2ui(state.open_gl.ui_shader_subroutines_index_uniform, subroutines[0], subroutines[1]);
	//glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_masked_rect(sys::state const& state, color_modification enabled, float x, float y, float width, float height,
		GLuint texture_handle, GLuint mask_texture_handle, ui::rotation r, bool flipped, bool rtl) {
	glBindVertexArray(state.open_gl.global_square_vao);

	bind_vertices_by_rotation(state, r, flipped, rtl);

	glUniform4f(state.open_gl.ui_shader_d_rect_uniform, x, y, width, height);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_handle);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mask_texture_handle);

	GLuint subroutines[2] = {map_color_modification_to_index(enabled), parameters::use_mask};
	glUniform2ui(state.open_gl.ui_shader_subroutines_index_uniform, subroutines[0], subroutines[1]);
	//glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_progress_bar(sys::state const& state, color_modification enabled, float progress, float x, float y, float width,
		float height, GLuint left_texture_handle, GLuint right_texture_handle, ui::rotation r, bool flipped, bool rtl) {
	glBindVertexArray(state.open_gl.global_square_vao);

	bind_vertices_by_rotation(state, r, flipped, rtl);

	glUniform4f(state.open_gl.ui_shader_d_rect_uniform, x, y, width, height);
	glUniform1f(state.open_gl.ui_shader_border_size_uniform, progress);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, left_texture_handle);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, right_texture_handle);

	GLuint subroutines[2] = {map_color_modification_to_index(enabled), parameters::progress_bar};
	glUniform2ui(state.open_gl.ui_shader_subroutines_index_uniform, subroutines[0], subroutines[1]);
	//glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_tinted_textured_rect(sys::state const& state, float x, float y, float width, float height, float r, float g, float b,
		GLuint texture_handle, ui::rotation rot, bool flipped, bool rtl) {
	glBindVertexArray(state.open_gl.global_square_vao);

	bind_vertices_by_rotation(state, rot, flipped, rtl);

	glUniform3f(state.open_gl.ui_shader_inner_color_uniform, r, g, b);
	glUniform4f(state.open_gl.ui_shader_d_rect_uniform, x, y, width, height);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_handle);

	GLuint subroutines[2] = {parameters::tint, parameters::no_filter};
	glUniform2ui(state.open_gl.ui_shader_subroutines_index_uniform, subroutines[0], subroutines[1]);
	//glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_tinted_rect(
	sys::state const& state,
	float x, float y, float width, float height,
	float r, float g, float b,
	ui::rotation rot, bool flipped, bool rtl
) {
	glBindVertexArray(state.open_gl.global_square_vao);
	bind_vertices_by_rotation(state, rot, flipped, rtl);
	glUniform3f(state.open_gl.ui_shader_inner_color_uniform, r, g, b);
	glUniform4f(state.open_gl.ui_shader_d_rect_uniform, x, y, width, height);
	GLuint subroutines[2] = { parameters::tint, parameters::transparent_color };
	glUniform2ui(state.open_gl.ui_shader_subroutines_index_uniform, subroutines[0], subroutines[1]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_tinted_subsprite(sys::state const& state, int frame, int total_frames, float x, float y,
		float width, float height, float r, float g, float b, GLuint texture_handle, ui::rotation rot, bool flipped,
		bool rtl) {
	glBindVertexArray(state.open_gl.global_square_vao);

	bind_vertices_by_rotation(state, rot, flipped, rtl);

	auto const scale = 1.0f / static_cast<float>(total_frames);
	glUniform3f(state.open_gl.ui_shader_inner_color_uniform, static_cast<float>(frame) * scale, scale, 0.0f);
	glUniform4f(state.open_gl.ui_shader_subrect_uniform, r, g, b, 0);
	glUniform4f(state.open_gl.ui_shader_d_rect_uniform, x, y, width, height);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_handle);

	GLuint subroutines[2] = { parameters::alternate_tint, parameters::sub_sprite };
	glUniform2ui(state.open_gl.ui_shader_subroutines_index_uniform, subroutines[0], subroutines[1]);
	//glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_subsprite(sys::state const& state, color_modification enabled, int frame, int total_frames, float x, float y,
		float width, float height, GLuint texture_handle, ui::rotation r, bool flipped, bool rtl) {
	glBindVertexArray(state.open_gl.global_square_vao);

	bind_vertices_by_rotation(state, r, flipped, rtl);

	auto const scale = 1.0f / static_cast<float>(total_frames);
	glUniform3f(state.open_gl.ui_shader_inner_color_uniform, static_cast<float>(frame) * scale, scale, 0.0f);
	glUniform4f(state.open_gl.ui_shader_d_rect_uniform, x, y, width, height);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_handle);

	GLuint subroutines[2] = {map_color_modification_to_index(enabled), parameters::sub_sprite};
	glUniform2ui(state.open_gl.ui_shader_subroutines_index_uniform, subroutines[0], subroutines[1]);
	//glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}


GLuint get_flag_texture_handle_from_tag(sys::state& state, const char tag[3]) {
	char ltag[3];
	ltag[0] = char(toupper(tag[0]));
	ltag[1] = char(toupper(tag[1]));
	ltag[2] = char(toupper(tag[2]));
	dcon::national_identity_id ident{};
	state.world.for_each_national_identity([&](dcon::national_identity_id id) {
		auto curr = nations::int_to_tag(state.world.national_identity_get_identifying_int(id));
		if(curr[0] == ltag[0] && curr[1] == ltag[1] && curr[2] == ltag[2]) {
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

void render_text_icon(sys::state& state, text::embedded_icon ico, float x, float baseline_y, float font_size, text::font& f, ogl::color_modification cmod) {
	float scale = 1.f;
	float icon_baseline = baseline_y + (f.internal_ascender / 64.f * font_size) - font_size;

	bind_vertices_by_rotation(state, ui::rotation::upright, false, false);
	glActiveTexture(GL_TEXTURE0);
	
	switch(ico) {
	case text::embedded_icon::army:
		scale = 1.3f;
		glBindTexture(GL_TEXTURE_2D, state.open_gl.army_icon_tex );
		break;
	case text::embedded_icon::navy:
		glBindTexture(GL_TEXTURE_2D,  state.open_gl.navy_icon_tex);
		scale = 1.3f;
		break;
	case text::embedded_icon::check:
		glBindTexture(GL_TEXTURE_2D, state.open_gl.checkmark_icon_tex);
		icon_baseline += font_size * 0.1f;
		break;
	case text::embedded_icon::xmark:
	{
		GLuint false_icon = (state.user_settings.color_blind_mode == sys::color_blind_mode::deutan || state.user_settings.color_blind_mode == sys::color_blind_mode::protan)
			? state.open_gl.color_blind_cross_icon_tex
			: state.open_gl.cross_icon_tex;
		glBindTexture(GL_TEXTURE_2D, false_icon);
		icon_baseline += font_size * 0.1f;
		break;
	}
	}

	GLuint icon_subroutines[2] = { map_color_modification_to_index(cmod), parameters::no_filter };
	glUniform2ui(state.open_gl.ui_shader_subroutines_index_uniform, icon_subroutines[0], icon_subroutines[1]);
	//glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, icon_subroutines); // must set all subroutines in one call
	glUniform4f(state.open_gl.ui_shader_d_rect_uniform, x, icon_baseline, scale * font_size, scale * font_size);
	glUniform4f(state.open_gl.ui_shader_subrect_uniform, 0.f, 1.f, 0.f, 1.f);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_text_flag(sys::state& state, text::embedded_flag ico, float x, float baseline_y, float font_size, text::font& f, ogl::color_modification cmod) {
	float icon_baseline = baseline_y + (f.internal_ascender / 64.f * font_size) - font_size;

	auto fat_id = dcon::fatten(state.world, ico.tag);
	auto nation = fat_id.get_nation_from_identity_holder();
	culture::flag_type flag_type = culture::flag_type{};
	if(bool(nation.id) && nation.get_owned_province_count() != 0) {
		flag_type = culture::get_current_flag_type(state, nation.id);
	} else {
		flag_type = culture::get_current_flag_type(state, ico.tag);
	}
	GLuint flag_texture_handle = ogl::get_flag_handle(state, ico.tag, flag_type);

	GLuint icon_subroutines[2] = { map_color_modification_to_index(cmod), parameters::no_filter };
	glUniform2ui(state.open_gl.ui_shader_subroutines_index_uniform, icon_subroutines[0], icon_subroutines[1]);
	//glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, icon_subroutines); // must set all subroutines in one call
	bind_vertices_by_rotation(state, ui::rotation::upright, false, false);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, flag_texture_handle);
	glUniform4f(state.open_gl.ui_shader_d_rect_uniform, x, icon_baseline + font_size * 0.15f, 1.5f * font_size * 0.9f,  font_size * 0.9f);
	glUniform4f(state.open_gl.ui_shader_subrect_uniform, 0.f, 1.f, 0.f, 1.f);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_text_unit_icon(sys::state& state, text::embedded_unit_icon ico, float x, float baseline_y, float font_size, text::font& f, ogl::color_modification cmod) {
	auto ascender_size = f.ascender(int32_t(font_size));
	auto top_adj = f.top_adjustment(int32_t(font_size));
	float icon_baseline = baseline_y + top_adj + ascender_size;
	//float icon_baseline = baseline_y + (f.internal_ascender / 64.f * font_size) - font_size;

	auto id = ico.unit_type;

	auto gfx_id =
		state.ui_defs.gui[
			state.ui_state.defs_by_name.find(
				state.lookup_key("gfx_storage_unit_types")
			)->second.definition
		].data.image.gfx_object;

	auto& gfx_def = state.ui_defs.gfx[gfx_id];

	auto frame = state.military_definitions.unit_base_definitions[id].icon - 1;

	render_subsprite(
		state,
		cmod,
		frame,
		gfx_def.number_of_frames,
		float(x) - ascender_size * 0.125f,
		icon_baseline - ascender_size,
		ascender_size * 1.25f,
		ascender_size * 1.25f,
		ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
		ui::rotation::upright,
		gfx_def.is_vertically_flipped(),
		false
	);
}

void render_commodity_icon(
	sys::state& state, dcon::commodity_id cid,
	float x, float y, float w, float h
) {
	auto gfx_id =
		state.ui_defs.gui[
			state.ui_state.defs_by_name.find(
				state.lookup_key("gfx_storage_commodity")
			)->second.definition
		].data.image.gfx_object;

	auto& gfx_def = state.ui_defs.gfx[gfx_id];

	auto frame = state.world.commodity_get_icon(cid);

	render_subsprite(
		state,
		ogl::color_modification::none,
		frame,
		gfx_def.number_of_frames,
		x,
		y,
		w,
		h,
		ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
		ui::rotation::upright,
		gfx_def.is_vertically_flipped(),
		false
	);
}

void render_text_commodity_icon(
	sys::state& state, text::embedded_commodity_icon ico,
	float x, float baseline_y,
	float font_size, text::font& f
) {
	float icon_baseline = baseline_y + (f.internal_ascender / 64.f * font_size) - font_size;

	render_commodity_icon(
		state, ico.commodity,
		x, baseline_y,
		font_size, font_size
	);
}

void internal_text_render(sys::state& state, text::stored_glyphs const& txt, float x, float baseline_y, float size, text::font& f) {
	GLuint subroutines[2] = { map_color_modification_to_index(ogl::color_modification::none), parameters::filter };
	glUniform2ui(state.open_gl.ui_shader_subroutines_index_uniform, subroutines[0], subroutines[1]);
	//glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	unsigned int glyph_count = static_cast<unsigned int>(txt.glyph_info.size());
	for(unsigned int i = 0; i < glyph_count; i++) {
		hb_codepoint_t glyphid = txt.glyph_info[i].codepoint;
		auto gso = f.glyph_positions[glyphid];
		float x_advance = float(txt.glyph_info[i].x_advance) / (float((1 << 6) * text::magnification_factor));
		float x_offset = float(txt.glyph_info[i].x_offset) / (float((1 << 6) * text::magnification_factor)) + float(gso.x);
		float y_offset = float(gso.y) - float(txt.glyph_info[i].y_offset) / (float((1 << 6) * text::magnification_factor));
		glBindVertexBuffer(0, state.open_gl.sub_square_buffers[gso.texture_slot & 63], 0, sizeof(GLfloat) * 4);
		assert(uint32_t(gso.texture_slot >> 6) < f.textures.size());
		assert(f.textures[gso.texture_slot >> 6]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, f.textures[gso.texture_slot >> 6]);
		glUniform4f(state.open_gl.ui_shader_d_rect_uniform, x + x_offset * size / 64.f, baseline_y + y_offset * size / 64.f, size, size);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		x += x_advance * size / 64.f;
		baseline_y -= (float(txt.glyph_info[i].y_advance) / (float((1 << 6) * text::magnification_factor))) * size / 64.f;
	}
}

void render_classic_text(sys::state& state, text::stored_glyphs const& txt, float x, float y, float size, color_modification enabled, color3f const& c, text::bm_font const& font, text::font& base_font) {
	std::string codepoints = "";
	for(uint32_t i = 0; i < uint32_t(txt.glyph_info.size()); i++) {
		codepoints.push_back(char(txt.glyph_info[i].codepoint));
	}
	uint32_t count = uint32_t(codepoints.length());

	float adv = 1.0f / font.width; // Font texture atlas spacing.
	bind_vertices_by_rotation(state, ui::rotation::upright, false, false);
	GLuint subroutines[2] = { map_color_modification_to_index(enabled), parameters::subsprite_b };
	glUniform2ui(state.open_gl.ui_shader_subroutines_index_uniform, subroutines[0], subroutines[1]);
	//glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

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
		uint8_t ch = uint8_t(codepoints[i]);
		if(i != 0 && i < count - 1 && ch == 0xC2 && uint8_t(codepoints[i + 1]) == 0xA3) {
			ch = 0xA3;
			i++;
		} else if(ch == 0xA4) {
			ch = 0xA3;
		}
		auto const& f = font.chars[ch];
		float CurX = x + f.x_offset;
		float CurY = y + f.y_offset;
		glUniform4f(state.open_gl.ui_shader_d_rect_uniform, CurX, CurY, float(f.width), float(f.height));
		glUniform3f(state.open_gl.ui_shader_inner_color_uniform, c.r, c.g, c.b);
		glUniform4f(state.open_gl.ui_shader_subrect_uniform, float(f.x) / float(font.width) /* x offset */,
				float(f.width) / float(font.width) /* x width */, float(f.y) / float(font.width) /* y offset */,
				float(f.height) / float(font.width) /* y height */
		);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		//float x_advance = float(txt.glyph_pos[i].x_advance) / (float((1 << 6) * text::magnification_factor));
		x += f.x_advance;
	}
}

void render_new_text(sys::state& state, text::stored_glyphs const& txt, color_modification enabled, float x, float y, float size, color3f const& c, text::font& f) {
	glUniform3f(state.open_gl.ui_shader_inner_color_uniform, c.r, c.g, c.b);
	glUniform1f(state.open_gl.ui_shader_border_size_uniform, 0.08f * 16.0f / size);
	internal_text_render(state, txt, x, y + size, size, f);
}

void render_text(sys::state& state, text::stored_glyphs const& txt, color_modification enabled, float x, float y, color3f const& c, uint16_t font_id) {
	auto& font = state.font_collection.get_font(state, text::font_index_from_font_id(state, font_id));
	if(state.user_settings.use_classic_fonts) {
		render_classic_text(state, txt, x, y, float(text::size_from_font_id(font_id)), enabled, c, text::get_bm_font(state, font_id), font);
		return;
	}
	render_new_text(state, txt, enabled, x, y, float(text::size_from_font_id(font_id)), c, font);
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

image load_stb_image(simple_fs::file& file) {
	int32_t file_channels = 4;
	int32_t size_x = 0;
	int32_t size_y = 0;
	auto content = simple_fs::view_contents(file);
	auto data = stbi_load_from_memory(reinterpret_cast<uint8_t const*>(content.data), int32_t(content.file_size), &size_x, &size_y, &file_channels, 4);
	return image(data, size_x, size_y, 4);
}

GLuint make_gl_texture(uint8_t* data, uint32_t size_x, uint32_t size_y, uint32_t channels) {
	GLuint texture_handle;
	glGenTextures(1, &texture_handle);
	const GLuint internalformats[] = { GL_R8, GL_RG8, GL_RGB8, GL_RGBA8 };
	const GLuint formats[] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };
	if(texture_handle) {
		glBindTexture(GL_TEXTURE_2D, texture_handle);
		glTexStorage2D(GL_TEXTURE_2D, 1, internalformats[channels - 1], size_x, size_y);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size_x, size_y, formats[channels - 1], GL_UNSIGNED_BYTE, data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	return texture_handle;
}
GLuint make_gl_texture(simple_fs::directory const& dir, native_string_view file_name) {
	auto file = open_file(dir, file_name);
	if(!file)
		return 0;
	auto image = load_stb_image(*file);
	return make_gl_texture(image.data, image.size_x, image.size_y, image.channels);
}

void set_gltex_parameters(GLuint texture_handle, GLuint texture_type, GLuint filter, GLuint wrap) {
	glBindTexture(texture_type, texture_handle);
	if(filter == GL_LINEAR_MIPMAP_NEAREST || filter == GL_LINEAR_MIPMAP_LINEAR) {
		glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(texture_type);
	} else {
		glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, filter);
		glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	glTexParameteri(texture_type, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(texture_type, GL_TEXTURE_WRAP_T, wrap);
	glBindTexture(texture_type, 0);
}
void set_gltex_parameters(GLuint texture_handle, GLuint texture_type, GLuint filter, GLuint wrap_a, GLuint wrap_b) {
	glBindTexture(texture_type, texture_handle);
	if(filter == GL_LINEAR_MIPMAP_NEAREST || filter == GL_LINEAR_MIPMAP_LINEAR) {
		glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(texture_type);
	} else {
		glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, filter);
		glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	glTexParameteri(texture_type, GL_TEXTURE_WRAP_S, wrap_a);
	glTexParameteri(texture_type, GL_TEXTURE_WRAP_T, wrap_b);
	glBindTexture(texture_type, 0);
}

GLuint load_texture_array_from_file(simple_fs::file& file, int32_t tiles_x, int32_t tiles_y) {
	auto image = load_stb_image(file);

	GLuint texture_handle = 0;
	glGenTextures(1, &texture_handle);
	if(texture_handle) {
		glBindTexture(GL_TEXTURE_2D_ARRAY, texture_handle);

		size_t p_dx = image.size_x / tiles_x; // Pixels of each tile in x
		size_t p_dy = image.size_y / tiles_y; // Pixels of each tile in y
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, GLsizei(p_dx), GLsizei(p_dy), GLsizei(tiles_x * tiles_y), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, image.size_x);
		glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, image.size_y);

		for(int32_t x = 0; x < tiles_x; x++)
			for(int32_t y = 0; y < tiles_y; y++)
				glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, GLint(x * tiles_x + y), GLsizei(p_dx), GLsizei(p_dy), 1, GL_RGBA, GL_UNSIGNED_BYTE, ((uint32_t const*)image.data) + (x * p_dy * image.size_x + y * p_dx));

		set_gltex_parameters(texture_handle, GL_TEXTURE_2D_ARRAY, GL_LINEAR_MIPMAP_NEAREST, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, 0);
	}
	return texture_handle;
}

void render_capture::ready(sys::state& state) {
	if(state.x_size > max_x || state.y_size > max_y) {
		max_x = std::max(max_x, state.x_size);
		max_y = std::max(max_y, state.y_size);

		if(texture_handle)
			glDeleteTextures(1, &texture_handle);
		if(framebuffer)
			glDeleteFramebuffers(1, &framebuffer);

		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		glGenTextures(1, &texture_handle);
		glBindTexture(GL_TEXTURE_2D, texture_handle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, max_x, max_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture_handle, 0);

		GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, DrawBuffers);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUseProgram(state.open_gl.ui_shader_program);
		glUniform1i(state.open_gl.ui_shader_texture_sampler_uniform, 0);
		glUniform1i(state.open_gl.ui_shader_secondary_texture_sampler_uniform, 1);
		glUniform1f(state.open_gl.ui_shader_screen_width_uniform, float(max_x) / state.user_settings.ui_scale);
		glUniform1f(state.open_gl.ui_shader_screen_height_uniform, float(max_y) / state.user_settings.ui_scale);
		glUniform1f(state.open_gl.ui_shader_gamma_uniform, state.user_settings.gamma);
		glViewport(0, 0, max_x, max_y);
		glDepthRange(-1.0f, 1.0f);
	} else {
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUseProgram(state.open_gl.ui_shader_program);
		glUniform1i(state.open_gl.ui_shader_texture_sampler_uniform, 0);
		glUniform1i(state.open_gl.ui_shader_secondary_texture_sampler_uniform, 1);
		glUniform1f(state.open_gl.ui_shader_screen_width_uniform, float(max_x) / state.user_settings.ui_scale);
		glUniform1f(state.open_gl.ui_shader_screen_height_uniform, float(max_y) / state.user_settings.ui_scale);
		glUniform1f(state.open_gl.ui_shader_gamma_uniform, state.user_settings.gamma);
		glViewport(0, 0, max_x, max_y);
		glDepthRange(-1.0f, 1.0f);
	}
}
void render_capture::finish(sys::state& state) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
GLuint render_capture::get() {
	return texture_handle;
}
render_capture::~render_capture() {
	if(texture_handle)
		glDeleteTextures(1, &texture_handle);
	if(framebuffer)
		glDeleteFramebuffers(1, &framebuffer);
}
void render_subrect(sys::state const& state, float target_x, float target_y, float target_width, float target_height, float source_x, float source_y, float source_width, float source_height, GLuint texture_handle) {
	bind_vertices_by_rotation(state, ui::rotation::upright, false, false);
	GLuint subroutines[2] = { parameters::enabled, parameters::subsprite_c };
	glUniform2ui(state.open_gl.ui_shader_subroutines_index_uniform, subroutines[0], subroutines[1]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_handle);

	glUniform4f(state.open_gl.ui_shader_d_rect_uniform, target_x, target_y, target_width, target_height);
	glUniform4f(state.open_gl.ui_shader_subrect_uniform, source_x /* x offset */, source_width /* x width */, source_y /* y offset */, source_height /* y height */);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void animation::start_animation(sys::state& state, int32_t x, int32_t y, int32_t w, int32_t h, type t, int32_t runtime) {
	start_state.ready(state);
	state.current_scene.get_root(state)->impl_render(state, 0, 0);
	start_state.finish(state);
	ani_type = t;
	x_pos = x;
	y_pos = y;
	x_size = w;
	y_size = h;
	ms_run_time = runtime;
	start_time = std::chrono::steady_clock::now();
	running = true;
}
void animation::post_update_frame(sys::state& state) {
	if(running) {
		bool needs_new_state = (ani_type == type::page_flip_left_rev || ani_type == type::page_flip_right_rev || ani_type == type::page_flip_up_rev);
		if(needs_new_state) {
			end_state.ready(state);
			state.current_scene.get_root(state)->impl_render(state, 0, 0);
			end_state.finish(state);
		}
	}
}
void animation::render(sys::state& state) {
	if(!running)
		return;
	auto entry_time = std::chrono::steady_clock::now();
	auto ms_count = std::chrono::duration_cast<std::chrono::milliseconds>(entry_time - start_time).count();
	if(ms_count > ms_run_time) {
		running = false;
	} else {
		float percent = float(ms_count) / float(ms_run_time);
		switch(ani_type) {
		case type::page_flip_left:
		{
			float extent = cos((percent) * 3.14159f / 2.0f);
			render_subrect(state, float(x_pos), float(y_pos), float(x_size * extent), float(y_size),
				float(x_pos) * state.user_settings.ui_scale / float(start_state.max_x), float(start_state.max_y - y_pos) * state.user_settings.ui_scale / float(start_state.max_y), float(x_size) * state.user_settings.ui_scale / float(start_state.max_x), float(-y_size) * state.user_settings.ui_scale / float(start_state.max_y),
				start_state.get());
		}
			break;
		case type::page_flip_right:
		{
			float extent = cos((percent) * 3.14159f / 2.0f);
			render_subrect(state, float(x_pos + (x_size * (1.0f - extent))), float(y_pos), float(x_size * extent), float(y_size),
				float(x_pos) * state.user_settings.ui_scale / float(start_state.max_x), float(start_state.max_y - y_pos) * state.user_settings.ui_scale / float(start_state.max_y), float(x_size) * state.user_settings.ui_scale / float(start_state.max_x), float(-y_size) * state.user_settings.ui_scale / float(start_state.max_y),
				start_state.get());
		}
			break;
		case type::page_flip_up:
		{
			float extent = cos((percent) * 3.14159f / 2.0f);
			render_subrect(state, float(x_pos), float(y_pos), float(x_size), float(y_size * extent),
				float(x_pos) * state.user_settings.ui_scale / float(start_state.max_x), float(start_state.max_y - y_pos) * state.user_settings.ui_scale / float(start_state.max_y), float(x_size) * state.user_settings.ui_scale / float(start_state.max_x), float(-y_size) * state.user_settings.ui_scale / float(start_state.max_y),
				start_state.get());
		}
			break;
		case type::page_flip_left_rev:
		{
			float extent = cos((1.0f - percent) * 3.14159f / 2.0f);
			render_subrect(state, float(x_pos), float(y_pos), float(x_size), float(y_size),
				float(x_pos) * state.user_settings.ui_scale / float(start_state.max_x), float(start_state.max_y - y_pos) * state.user_settings.ui_scale / float(start_state.max_y), float(x_size) * state.user_settings.ui_scale / float(start_state.max_x), float(-y_size) * state.user_settings.ui_scale / float(start_state.max_y),
				start_state.get());
			render_subrect(state, float(x_pos), float(y_pos), float(x_size * extent), float(y_size),
				float(x_pos) * state.user_settings.ui_scale / float(end_state.max_x), float(start_state.max_y - y_pos) * state.user_settings.ui_scale / float(end_state.max_y), float(x_size) * state.user_settings.ui_scale / float(end_state.max_x), float(-y_size) * state.user_settings.ui_scale / float(end_state.max_y),
				end_state.get());
		}
			break;
		case type::page_flip_right_rev:
		{
			float extent = cos((1.0f - percent) * 3.14159f / 2.0f);
			render_subrect(state, float(x_pos), float(y_pos), float(x_size), float(y_size),
				float(x_pos) * state.user_settings.ui_scale / float(start_state.max_x), float(start_state.max_y - y_pos) * state.user_settings.ui_scale / float(start_state.max_y), float(x_size) * state.user_settings.ui_scale / float(start_state.max_x), float(-y_size) * state.user_settings.ui_scale / float(start_state.max_y),
				start_state.get());
			render_subrect(state, float(x_pos + (x_size * (1.0f - extent))), float(y_pos), float(x_size * extent), float(y_size),
				float(x_pos) * state.user_settings.ui_scale / float(end_state.max_x), float(end_state.max_y - y_pos) * state.user_settings.ui_scale / float(end_state.max_y), float(x_size) * state.user_settings.ui_scale / float(end_state.max_x), float(-y_size) * state.user_settings.ui_scale / float(end_state.max_y),
				end_state.get());
		}
			break;
		case type::page_flip_up_rev:
		{
			float extent = cos((1.0f - percent) * 3.14159f / 2.0f);
			render_subrect(state, float(x_pos), float(y_pos), float(x_size), float(y_size),
				float(x_pos) * state.user_settings.ui_scale / float(start_state.max_x), float(start_state.max_y - y_pos) * state.user_settings.ui_scale / float(start_state.max_y), float(x_size) * state.user_settings.ui_scale / float(start_state.max_x), float(-y_size) * state.user_settings.ui_scale / float(start_state.max_y),
				start_state.get());
			render_subrect(state, float(x_pos), float(y_pos), float(x_size), float(y_size * extent),
				float(x_pos) * state.user_settings.ui_scale / float(end_state.max_x), float(end_state.max_y - y_pos) * state.user_settings.ui_scale / float(end_state.max_y), float(x_size) * state.user_settings.ui_scale / float(end_state.max_x), float(-y_size) * state.user_settings.ui_scale / float(end_state.max_y),
				end_state.get());
		}
			break;
		}
	}
}

} // namespace ogl
