#include "opengl_wrapper.hpp"
#include "system_state.hpp"
#include "simple_fs.hpp"

namespace ogl {
void notify_user_of_fatal_opengl_error(std::string message) {
#ifdef _WIN64
	MessageBoxA(nullptr, message.c_str(), "OpenGL error", MB_OK);
#else
	printf("OpenGL error: %s", message.c_str());
#endif
	std::abort();
}

void intialize_opengl(sys::state& state) {
	create_opengl_context(state);

	load_shaders(state); // create shaders
	load_global_squares(state); // create various squares to drive the shaders with


	glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_SCISSOR_TEST);
	glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
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
	auto ui_fshader = open_file(root, NATIVE("ui_f_shader.txt"));
	auto ui_vshader = open_file(root, NATIVE("ui_v_shader.txt"));
	if(bool(ui_fshader) && bool(ui_fshader)) {
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
	glGenBuffers(1, &state.open_gl.global_sqaure_buffer);

	// Populate the position buffer
	glBindBuffer(GL_ARRAY_BUFFER, state.open_gl.global_sqaure_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_square_data, GL_STATIC_DRAW);

	glGenVertexArrays(1, &state.open_gl.global_square_vao);
	glBindVertexArray(state.open_gl.global_square_vao);
	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //texture coordinates

	glBindVertexBuffer(0, state.open_gl.global_sqaure_buffer, 0, sizeof(GLfloat) * 4);

	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, 0); //position
	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2); //texture coordinates
	glVertexAttribBinding(0, 0); //position -> to array zero
	glVertexAttribBinding(1, 0); //texture coordinates -> to array zero 


	glGenBuffers(1, &state.open_gl.global_sqaure_left_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, state.open_gl.global_sqaure_left_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_square_left_data, GL_STATIC_DRAW);

	glGenBuffers(1, &state.open_gl.global_sqaure_right_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, state.open_gl.global_sqaure_right_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_square_right_data, GL_STATIC_DRAW);


	glGenBuffers(1, &state.open_gl.global_sqaure_right_flipped_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, state.open_gl.global_sqaure_right_flipped_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_square_right_flipped_data, GL_STATIC_DRAW);

	glGenBuffers(1, &state.open_gl.global_sqaure_left_flipped_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, state.open_gl.global_sqaure_left_flipped_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_square_left_flipped_data, GL_STATIC_DRAW);

	glGenBuffers(1, &state.open_gl.global_sqaure_flipped_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, state.open_gl.global_sqaure_flipped_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 16, global_square_flipped_data, GL_STATIC_DRAW);


	glGenBuffers(64, state.open_gl.sub_sqaure_buffers);
	for(uint32_t i = 0; i < 64; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, state.open_gl.sub_sqaure_buffers[i]);

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

}
