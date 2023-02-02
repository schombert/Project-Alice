#include "opengl_wrapper.hpp"
#include "system_state.hpp"
#include "simple_fs.hpp"

namespace ogl {
void notify_user_of_fatal_opengl_error(std::string message) {
#ifdef _WIN64
	MessageBoxA(nullptr, message.c_str(), "OpenGL error", MB_OK);
#else
	fprintf(stderr, "OpenGL error: %s\n", message.c_str());
#endif
	std::abort();
}

void initialize_opengl(sys::state& state) {
	create_opengl_context(state);

	glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
	glEnable(GL_LINE_SMOOTH);

	load_shaders(state); // create shaders
	load_global_squares(state); // create various squares to drive the shaders with



	//glEnable(GL_SCISSOR_TEST);

}

GLint compile_shader(std::string_view source, GLenum type) {
	GLuint return_value = glCreateShader(type);

	if(return_value == 0) {
		notify_user_of_fatal_opengl_error("shader creation failed");
	}
	GLchar const* texts[] = { source.data() };
	GLint lengths[] = { GLint(source.length()) };

	glShaderSource(return_value, 1, texts, lengths);
	glCompileShader(return_value);

	GLint result;
	glGetShaderiv(return_value, GL_COMPILE_STATUS, &result);
	if(result == GL_FALSE) {
		GLint logLen;
		glGetShaderiv(return_value, GL_INFO_LOG_LENGTH, &logLen);

		char* log = new char[static_cast<size_t>(logLen)];
		GLsizei written = 0;
		glGetShaderInfoLog(return_value, logLen, &written, log);
		notify_user_of_fatal_opengl_error(std::string("Shader failed to compile:\n") + log);
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

static GLfloat global_square_data[] = {
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 0.0f, 1.0f, 0.0f
};

static GLfloat global_square_right_data[] = {
	0.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 0.0f
};

static GLfloat global_square_left_data[] = {
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f, 1.0f
};


static GLfloat global_square_flipped_data[] = {
	0.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 1.0f, 1.0f
};

static GLfloat global_square_right_flipped_data[] = {
	0.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f
};

static GLfloat global_square_left_flipped_data[] = {
	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 1.0f, 0.0f
};

void load_shaders(sys::state& state) {
	auto root = get_root(state.common_fs);
	auto ui_fshader = open_file(root, NATIVE("assets/shaders/ui_f_shader.glsl"));
	auto ui_vshader = open_file(root, NATIVE("assets/shaders/ui_v_shader.glsl"));
	if(bool(ui_fshader) && bool(ui_vshader)) {
		auto vertex_content = view_contents(*ui_vshader);
		auto fragment_content = view_contents(*ui_fshader);
		state.open_gl.ui_shader_program = create_program(
			std::string_view(vertex_content.data, vertex_content.file_size),
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
	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //texture coordinates

	glBindVertexBuffer(0, state.open_gl.global_square_buffer, 0, sizeof(GLfloat) * 4);

	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, 0); //position
	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2); //texture coordinates
	glVertexAttribBinding(0, 0); //position -> to array zero
	glVertexAttribBinding(1, 0); //texture coordinates -> to array zero


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

		const float cell_x = static_cast<float>(i & 7) / 8.0f;
		const float cell_y = static_cast<float>((i >> 3) & 7) / 8.0f;

		GLfloat global_sub_square_data[] = {
			0.0f, 0.0f, cell_x, cell_y,
			0.0f, 1.0f, cell_x, cell_y + 1.0f / 8.0f,
			1.0f, 1.0f, cell_x + 1.0f / 8.0f, cell_y + 1.0f / 8.0f,
			1.0f, 0.0f, cell_x + 1.0f / 8.0f, cell_y
		};

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

void render_textured_rect(sys::state const& state, color_modification enabled, float x, float y, float width, float height, GLuint texture_handle, ui::rotation r, bool flipped) {
	glBindVertexArray(state.open_gl.global_square_vao);

	bind_vertices_by_rotation(state, r, flipped);

	glUniform4f(parameters::drawing_rectangle, x, y, width, height);
	//glUniform4f(parameters::drawing_rectangle, 0, 0, width, height);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_handle);

	GLuint subroutines[2] = { map_color_modification_to_index(enabled), parameters::no_filter };
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_textured_rect_direct(sys::state const& state, float x, float y, float width, float height, uint32_t handle) {
	glBindVertexArray(state.open_gl.global_square_vao);

	glBindVertexBuffer(0, state.open_gl.global_square_buffer, 0, sizeof(GLfloat) * 4);

	glUniform4f(parameters::drawing_rectangle, x, y, width, height);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, handle);

	GLuint subroutines[2] = { parameters::enabled, parameters::no_filter };
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_linegraph(sys::state const& state, color_modification enabled, float x, float y, float width, float height, lines& l) {
	glBindVertexArray(state.open_gl.global_square_vao);

	l.bind_buffer();

	glUniform4f(parameters::drawing_rectangle, x, y, width, height);
	glLineWidth(2.0f);

	GLuint subroutines[2] = { map_color_modification_to_index(enabled), parameters::linegraph };
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(l.count));
}

void render_barchart(sys::state const& state, color_modification enabled, float x, float y, float width, float height, data_texture& t, ui::rotation r, bool flipped) {
	glBindVertexArray(state.open_gl.global_square_vao);

	bind_vertices_by_rotation(state, r, flipped);


	glUniform4f(parameters::drawing_rectangle, x, y, width, height);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, t.handle());

	GLuint subroutines[2] = { map_color_modification_to_index(enabled), parameters::barchart };
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_piechart(sys::state const& state, color_modification enabled, float x, float y, float size, data_texture& t) {
	glBindVertexArray(state.open_gl.global_square_vao);

	glBindVertexBuffer(0, state.open_gl.global_square_buffer, 0, sizeof(GLfloat) * 4);

	glUniform4f(parameters::drawing_rectangle, x, y, size, size);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, t.handle());

	GLuint subroutines[2] = { map_color_modification_to_index(enabled), parameters::piechart };
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_bordered_rect(sys::state const& state, color_modification enabled, float border_size, float x, float y, float width, float height, GLuint texture_handle, ui::rotation r, bool flipped) {
	glBindVertexArray(state.open_gl.global_square_vao);

	bind_vertices_by_rotation(state, r, flipped);


	glUniform4f(parameters::drawing_rectangle, x, y, width, height);
	glUniform1f(parameters::border_size, border_size);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_handle);

	GLuint subroutines[2] = { map_color_modification_to_index(enabled), parameters::frame_stretch };
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_masked_rect(sys::state const& state, color_modification enabled, float x, float y, float width, float height, GLuint texture_handle, GLuint mask_texture_handle, ui::rotation r, bool flipped) {
	glBindVertexArray(state.open_gl.global_square_vao);

	bind_vertices_by_rotation(state, r, flipped);


	glUniform4f(parameters::drawing_rectangle, x, y, width, height);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_handle);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mask_texture_handle);

	GLuint subroutines[2] = { map_color_modification_to_index(enabled), parameters::use_mask };
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_progress_bar(sys::state const& state, color_modification enabled, float progress, float x, float y, float width, float height, GLuint left_texture_handle, GLuint right_texture_handle, ui::rotation r, bool flipped) {
	glBindVertexArray(state.open_gl.global_square_vao);

	bind_vertices_by_rotation(state, r, flipped);


	glUniform4f(parameters::drawing_rectangle, x, y, width, height);
	glUniform1f(parameters::border_size, progress);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, left_texture_handle);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, right_texture_handle);

	GLuint subroutines[2] = { map_color_modification_to_index(enabled), parameters::progress_bar };
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_tinted_textured_rect(sys::state const& state, float x, float y, float width, float height, float r, float g, float b, GLuint texture_handle, ui::rotation rot, bool flipped) {
	glBindVertexArray(state.open_gl.global_square_vao);

	bind_vertices_by_rotation(state, rot, flipped);

	glUniform3f(parameters::inner_color, r, g, b);
	glUniform4f(parameters::drawing_rectangle, x, y, width, height);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_handle);

	GLuint subroutines[2] = { parameters::tint, parameters::no_filter };
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_subsprite(sys::state const& state, color_modification enabled, int frame, int total_frames, float x, float y, float width, float height, GLuint texture_handle, ui::rotation r, bool flipped) {
	glBindVertexArray(state.open_gl.global_square_vao);

	bind_vertices_by_rotation(state, r, flipped);

	const auto scale = 1.0f / static_cast<float>(total_frames);
	glUniform3f(parameters::inner_color, static_cast<float>(frame) * scale, scale, 0.0f);
	glUniform4f(parameters::drawing_rectangle, x, y, width, height);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_handle);

	GLuint subroutines[2] = { map_color_modification_to_index(enabled), parameters::sub_sprite };
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}


void render_character(sys::state const& state, char codepoint, color_modification enabled, float x, float y, float size, text::font& f) {
	if(text::win1250toUTF16(codepoint) != ' ') {
		f.make_glyph(codepoint);

		glBindVertexBuffer(0, state.open_gl.sub_square_buffers[uint8_t(codepoint) & 63], 0, sizeof(GLfloat) * 4);
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

void internal_text_render(sys::state const& state, char const* codepoints, uint32_t count, float x, float baseline_y, float size, text::font& f) {
	for(uint32_t i = 0; i < count; ++i) {
		if(text::win1250toUTF16(codepoints[i]) != ' ') {
			f.make_glyph(codepoints[i]);

			glBindVertexBuffer(0, state.open_gl.sub_square_buffers[uint8_t(codepoints[i]) & 63], 0, sizeof(GLfloat) * 4);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, f.textures[uint8_t(codepoints[i]) >> 6]);

			glUniform4f(parameters::drawing_rectangle, x + f.glyph_positions[uint8_t(codepoints[i])].x * size / 64.0f, baseline_y + f.glyph_positions[uint8_t(codepoints[i])].y * size / 64.0f, size, size);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}
		x += f.glyph_advances[uint8_t(codepoints[i])] * size / 64.0f + ((i != count - 1) ? f.kerning(codepoints[i], codepoints[i + 1]) * size / 64.0f : 0.0f);
	}
}


void render_text(sys::state const& state, char const* codepoints, uint32_t count, color_modification enabled, float x, float y, float size, const color3f& c, text::font& f) {
	GLuint subroutines[2] = { map_color_modification_to_index(enabled), parameters::filter };
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines);

	glUniform3f(parameters::inner_color, c.r, c.g, c.b);
	glUniform1f(parameters::border_size, 0.08f * 16.0f / size);

	internal_text_render(state, codepoints, count, x, y + size, size, f);
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

}
