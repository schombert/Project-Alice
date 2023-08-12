#include "opengl_wrapper.hpp"
#include "system_state.hpp"
#include "simple_fs.hpp"

namespace ogl {

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
		for(uint32_t i = 0; i < state.culture_definitions.governments.size(); ++i) {
			const auto gov_id = dcon::government_type_id(dcon::government_type_id::value_base_t(i));
			state.flag_types.push_back(state.culture_definitions.governments[gov_id].flag);
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
}

GLint compile_shader(std::string_view source, GLenum type) {
	GLuint return_value = glCreateShader(type);

	if(return_value == 0) {
		report::fatal_error("Shader creation failed");
	}
	GLchar const* texts[] = {source.data()};
	GLint lengths[] = {GLint(source.length())};

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
		report::fatal_error(std::string("Shader failed to compile:\n") + log);
	}
	return return_value;
}

GLuint create_program(std::string_view vertex_shader, std::string_view fragment_shader) {
	GLuint return_value = glCreateProgram();
	if(return_value == 0) {
		report::fatal_error("Program creation failed");
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
		report::fatal_error(std::string("Program failed to link:\n") + log);
	}

	glDeleteShader(v_shader);
	glDeleteShader(f_shader);

	return return_value;
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
		report::fatal_error("Unable to open a necessary shader file");
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

void render_character(sys::state const& state, char codepoint, color_modification enabled, float x, float y, float size,
		text::font& f) {
	if(text::win1250toUTF16(codepoint) != ' ') {
		// f.make_glyph(codepoint);

		glBindVertexBuffer(0, state.open_gl.sub_square_buffers[uint8_t(codepoint) & 63], 0, sizeof(GLfloat) * 4);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, f.textures[uint8_t(codepoint) >> 6]);

		glUniform4f(parameters::drawing_rectangle, x, y, size, size);
		glUniform3f(parameters::inner_color, 0.0f, 0.0f, 0.0f);
		glUniform1f(parameters::border_size, 0.08f * 16.0f / size);

		GLuint subroutines[2] = {map_color_modification_to_index(enabled), parameters::border_filter};
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines); // must set all subroutines in one call

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}
}

void internal_text_render(sys::state const& state, char const* codepoints, uint32_t count, float x, float baseline_y, float size,
		text::font& f, GLuint const* subroutines, GLuint const* icon_subroutines) {
	for(uint32_t i = 0; i < count; ++i) {
		if(text::win1250toUTF16(codepoints[i]) != ' ') {
			// f.make_glyph(codepoints[i]);
			if(text::win1250toUTF16(codepoints[i]) != u'\u0001' && text::win1250toUTF16(codepoints[i]) != u'\u0002') {
				glBindVertexBuffer(0, state.open_gl.sub_square_buffers[uint8_t(codepoints[i]) & 63], 0, sizeof(GLfloat) * 4);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, f.textures[uint8_t(codepoints[i]) >> 6]);
				glUniform4f(parameters::drawing_rectangle, x + f.glyph_positions[uint8_t(codepoints[i])].x * size / 64.0f,
						baseline_y + f.glyph_positions[uint8_t(codepoints[i])].y * size / 64.0f, size, size);
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

				x += f.glyph_advances[uint8_t(codepoints[i])] * size / 64.0f +
						 ((i != count - 1) ? f.kerning(codepoints[i], codepoints[i + 1]) * size / 64.0f : 0.0f);
			} else {
				bind_vertices_by_rotation(state, ui::rotation::upright, false);
				glActiveTexture(GL_TEXTURE0);
				glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, icon_subroutines);
				glBindTexture(GL_TEXTURE_2D,
						text::win1250toUTF16(codepoints[i]) == u'\u0001' ? state.open_gl.cross_icon_tex : state.open_gl.checkmark_icon_tex);
				glUniform4f(parameters::drawing_rectangle, x + f.glyph_positions[0x4D].x * size / 64.0f,
						baseline_y + f.glyph_positions[0x4D].y * size / 64.0f, size, size);
				glUniform4f(ogl::parameters::subrect, 0.f /* x offset */, 1.f /* x width */, 0.f /* y offset */, 1.f /* y height */
				);
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
				glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines);

				x +=
						f.glyph_advances[0x4D] * size / 64.0f + ((i != count - 1) ? f.kerning(0x4D, codepoints[i + 1]) * size / 64.0f : 0.0f);
			}
		} else {
			x += f.glyph_advances[uint8_t(codepoints[i])] * size / 64.0f +
					 ((i != count - 1) ? f.kerning(codepoints[i], codepoints[i + 1]) * size / 64.0f : 0.0f);
		}
	}
}

void render_new_text(sys::state const& state, char const* codepoints, uint32_t count, color_modification enabled, float x,
		float y, float size, color3f const& c, text::font& f) {
	glUniform3f(parameters::inner_color, c.r, c.g, c.b);
	glUniform1f(parameters::border_size, 0.08f * 16.0f / size);

	GLuint subroutines[2] = {map_color_modification_to_index(enabled), parameters::filter};
	GLuint icon_subroutines[2] = {map_color_modification_to_index(enabled), parameters::no_filter};
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines);
	internal_text_render(state, codepoints, count, x, y + size, size, f, subroutines, icon_subroutines);
}

void render_classic_text(sys::state const& state, float x, float y, char const* codepoints, uint32_t count,
		color_modification enabled, color3f const& c, text::BMFont const& font) {
	float adv = (float)1.0 / font.Width; // Font texture atlas spacing.

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
		auto f = font.Chars[0];
		if(uint8_t(codepoints[i]) == 0xA4 || uint8_t(codepoints[i]) == 0x01 || uint8_t(codepoints[i]) == 0x02) {
			GLuint money_subroutines[2] = {map_color_modification_to_index(enabled), parameters::no_filter};
			glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, money_subroutines);
			f = font.Chars[0x4D];
			float scaling = uint8_t(codepoints[i]) == 0xA4 ? 1.5f : 1.f;
			float offset = uint8_t(codepoints[i]) == 0xA4 ? 0.25f : 0.f;
			float CurX = x + f.XOffset - (float(f.Width) * offset);
			float CurY = y + f.YOffset - (float(f.Height) * offset);
			glUniform4f(ogl::parameters::drawing_rectangle, CurX, CurY, float(f.Width) * scaling, float(f.Height) * scaling);
			glBindTexture(GL_TEXTURE_2D, uint8_t(codepoints[i]) == 0xA4		? state.open_gl.money_icon_tex
																	 : uint8_t(codepoints[i]) == 0x01 ? state.open_gl.cross_icon_tex
																																		: state.open_gl.checkmark_icon_tex);
			glUniform3f(parameters::inner_color, c.r, c.g, c.b);
			glUniform4f(ogl::parameters::subrect, float(f.x) / float(font.Width) /* x offset */,
					float(f.Width) / float(font.Width) /* x width */, float(f.y) / float(font.Width) /* y offset */,
					float(f.Height) / float(font.Width) /* y height */
			);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			// Restore affected state
			glBindTexture(GL_TEXTURE_2D, font.ftexid);
			glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, subroutines);
		} else {
			f = font.Chars[uint8_t(codepoints[i])];
			float CurX = x + f.XOffset;
			float CurY = y + f.YOffset;
			glUniform4f(ogl::parameters::drawing_rectangle, CurX, CurY, float(f.Width), float(f.Height));
			glUniform3f(parameters::inner_color, c.r, c.g, c.b);
			glUniform4f(ogl::parameters::subrect, float(f.x) / float(font.Width) /* x offset */,
					float(f.Width) / float(font.Width) /* x width */, float(f.y) / float(font.Width) /* y offset */,
					float(f.Height) / float(font.Width) /* y height */
			);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		// Only check kerning if there is greater then 1 character and
		// if the check character is 1 less then the end of the string.
		if(i != count - 1) {
			x += font.GetKerningPair(codepoints[i], codepoints[i + 1]);
		}
		x += f.XAdvance;
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

} // namespace ogl
