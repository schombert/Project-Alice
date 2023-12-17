#include "map.hpp"
#include "glm/fwd.hpp"
#include "texture.hpp"
#include "province.hpp"
#include <cmath>
#include <numbers>
#include <glm/glm.hpp>
#include <glm/mat3x3.hpp>
#include <unordered_map>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/polar_coordinates.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/transform.hpp>

#include "stb_image.h"
#include "system_state.hpp"
#include "parsers_declarations.hpp"
#include "math_fns.hpp"

#include "xac.hpp"
#include "xac.cpp"

namespace map {

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

GLuint load_texture_array_from_file(simple_fs::file& file, int32_t tiles_x, int32_t tiles_y) {
	auto image = load_stb_image(file);

	GLuint texture_handle;
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


void display_data::update_borders(sys::state& state) {
	uint32_t border_id = 0;
	for(; border_id < state.world.province_adjacency_size(); border_id++) {
		auto& border = borders[border_id];
		border.type_flag = state.world.province_adjacency_get_type(dcon::province_adjacency_id(dcon::province_adjacency_id::value_base_t(border_id)));
	}
	for(; border_id < borders.size(); border_id++) {
		auto& border = borders[border_id];
		border.type_flag = province::border::test_bit;
	}
}

void add_nation_visible_provinces(sys::state& state, std::vector<dcon::province_id>& list, dcon::nation_id n) {
	for(auto pc : state.world.nation_get_province_control_as_nation(n))
		list.push_back(pc.get_province());
	for(auto ac : state.world.nation_get_army_control_as_controller(n))
		list.push_back(ac.get_army().get_location_from_army_location());
	for(auto nc : state.world.nation_get_navy_control_as_controller(n))
		list.push_back(nc.get_navy().get_location_from_navy_location());
}

void display_data::update_fog_of_war(sys::state& state) {
	std::vector<dcon::province_id> direct_provinces;
	add_nation_visible_provinces(state, direct_provinces, state.local_player_nation);
	for(auto urel : state.world.nation_get_overlord_as_ruler(state.local_player_nation))
		add_nation_visible_provinces(state, direct_provinces, urel.get_subject());
	for(auto rel : state.world.nation_get_diplomatic_relation(state.local_player_nation)) {
		if(rel.get_are_allied()) {
			auto n = rel.get_related_nations(0) == state.local_player_nation ? rel.get_related_nations(1) : rel.get_related_nations(0);
			add_nation_visible_provinces(state, direct_provinces, n);
			for(auto urel : state.world.nation_get_overlord_as_ruler(n))
				add_nation_visible_provinces(state, direct_provinces, urel.get_subject());
		}
	}

	// update fog of war too
	std::vector<uint32_t> province_fows(state.world.province_size() + 1, 0xFFFFFFFF);
	if(state.user_settings.fow_enabled || state.network_mode != sys::network_mode_type::single_player) {
		state.map_state.visible_provinces.clear();
		state.map_state.visible_provinces.resize(state.world.province_size() + 1, false);
		for(auto p : direct_provinces) {
			if(bool(p)) {
				state.map_state.visible_provinces[province::to_map_id(p)] = true;
				for(auto c : state.world.province_get_province_adjacency(p)) {
					auto pc = c.get_connected_provinces(0) == p ? c.get_connected_provinces(1) : c.get_connected_provinces(0);
					if(bool(pc)) {
						state.map_state.visible_provinces[province::to_map_id(pc)] = true;
					}
				}
			}
		}
		for(auto p : state.world.in_province)
			province_fows[province::to_map_id(p)] = uint32_t(state.map_state.visible_provinces[province::to_map_id(p)] ? 0xFFFFFFFF : 0x7B7B7B7B);
		gen_prov_color_texture(province_fow, province_fows);
	} else {
		state.map_state.visible_provinces.clear();
		state.map_state.visible_provinces.resize(state.world.province_size() + 1, true);
		gen_prov_color_texture(province_fow, province_fows);
	}
}

void create_textured_line_vbo(GLuint& vbo, std::vector<textured_line_vertex>& data) {
	// Create and populate the border VBO
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	if(!data.empty())
		glBufferData(GL_ARRAY_BUFFER, sizeof(textured_line_vertex) * data.size(), data.data(), GL_STATIC_DRAW);

	// Bind the VBO to 0 of the VAO
	glBindVertexBuffer(0, vbo, 0, sizeof(textured_line_vertex));

	// Set up vertex attribute format for the position
	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, offsetof(textured_line_vertex, position_));
	// Set up vertex attribute format for the normal direction
	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, offsetof(textured_line_vertex, normal_direction_));
	// Set up vertex attribute format for the direction
	glVertexAttribFormat(2, 1, GL_FLOAT, GL_FALSE, offsetof(textured_line_vertex, texture_coordinate_));
	// Set up vertex attribute format for the texture coordinates
	glVertexAttribFormat(3, 1, GL_FLOAT, GL_FALSE, offsetof(textured_line_vertex, distance_));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glVertexAttribBinding(0, 0);
	glVertexAttribBinding(1, 0);
	glVertexAttribBinding(2, 0);
	glVertexAttribBinding(3, 0);
}

void create_unit_arrow_vbo(GLuint& vbo, std::vector<curved_line_vertex>& data) {
	// Create and populate the border VBO
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	if(!data.empty())
		glBufferData(GL_ARRAY_BUFFER, sizeof(curved_line_vertex) * data.size(), data.data(), GL_STATIC_DRAW);

	// Bind the VBO to 0 of the VAO
	glBindVertexBuffer(0, vbo, 0, sizeof(curved_line_vertex));

	// Set up vertex attribute format for the position
	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, offsetof(curved_line_vertex, position_));
	// Set up vertex attribute format for the normal direction
	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, offsetof(curved_line_vertex, normal_direction_));
	// Set up vertex attribute format for the direction
	glVertexAttribFormat(2, 2, GL_FLOAT, GL_FALSE, offsetof(curved_line_vertex, direction_));
	// Set up vertex attribute format for the texture coordinates
	glVertexAttribFormat(3, 2, GL_FLOAT, GL_FALSE, offsetof(curved_line_vertex, texture_coord_));
	// Set up vertex attribute format for the type
	glVertexAttribFormat(4, 1, GL_FLOAT, GL_FALSE, offsetof(curved_line_vertex, type_));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glVertexAttribBinding(0, 0);
	glVertexAttribBinding(1, 0);
	glVertexAttribBinding(2, 0);
	glVertexAttribBinding(3, 0);
	glVertexAttribBinding(4, 0);
}

void create_text_line_vbo(GLuint& vbo) {
	// Create and populate the border VBO
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// Bind the VBO to 0 of the VAO
	glBindVertexBuffer(0, vbo, 0, sizeof(text_line_vertex));

	// Set up vertex attribute format for the position
	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, offsetof(text_line_vertex, position_));
	// Set up vertex attribute format for the normal direction
	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, offsetof(text_line_vertex, normal_direction_));
	// Set up vertex attribute format for the direction
	glVertexAttribFormat(2, 2, GL_FLOAT, GL_FALSE, offsetof(text_line_vertex, direction_));
	// Set up vertex attribute format for the texture coordinates
	glVertexAttribFormat(3, 2, GL_FLOAT, GL_FALSE, offsetof(text_line_vertex, texture_coord_));
	glVertexAttribFormat(4, 1, GL_FLOAT, GL_FALSE, offsetof(text_line_vertex, type_));
	glVertexAttribFormat(5, 1, GL_FLOAT, GL_FALSE, offsetof(text_line_vertex, thickness_));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glVertexAttribBinding(0, 0);
	glVertexAttribBinding(1, 0);
	glVertexAttribBinding(2, 0);
	glVertexAttribBinding(3, 0);
	glVertexAttribBinding(4, 0);
	glVertexAttribBinding(5, 0);
}

void create_drag_box_vbo(GLuint& vbo) {
	// Create and populate the border VBO
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// Bind the VBO to 0 of the VAO
	glBindVertexBuffer(0, vbo, 0, sizeof(screen_vertex));

	// Set up vertex attribute format for the position
	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, offsetof(screen_vertex, position_));
	glEnableVertexAttribArray(0);
	glVertexAttribBinding(0, 0);
}

void display_data::create_border_ogl_objects() {
	// Create and bind the VAO
	glGenVertexArrays(1, &border_vao);
	glBindVertexArray(border_vao);
	create_unit_arrow_vbo(border_vbo, border_vertices);

	glGenVertexArrays(1, &river_vao);
	glBindVertexArray(river_vao);
	create_textured_line_vbo(river_vbo, river_vertices);

	create_textured_line_vbo(coastal_border_vbo, coastal_vertices);

	glGenVertexArrays(1, &unit_arrow_vao);
	glBindVertexArray(unit_arrow_vao);
	create_unit_arrow_vbo(unit_arrow_vbo, unit_arrow_vertices);

	glGenVertexArrays(1, &text_line_vao);
	glBindVertexArray(text_line_vao);
	create_text_line_vbo(text_line_vbo);

	glGenVertexArrays(1, &drag_box_vao);
	glBindVertexArray(drag_box_vao);
	create_drag_box_vbo(drag_box_vbo);

	glBindVertexArray(0);
}

void display_data::create_meshes() {

	std::vector<map_vertex> land_vertices;

	auto add_quad = [map_size = glm::vec2(float(size_x), float(size_y))](std::vector<map_vertex>& vertices, glm::vec2 pos0, glm::vec2 pos1) {
		// Rescale the coordinate to 0-1
		pos0 /= map_size;
		pos1 /= map_size;

		// First vertex of the quad
		vertices.emplace_back(pos0.x, pos0.y);
		vertices.emplace_back(pos1.x, pos0.y);
		vertices.emplace_back(pos1.x, pos1.y);
		// Second vertex of the quad
		vertices.emplace_back(pos1.x, pos1.y);
		vertices.emplace_back(pos0.x, pos1.y);
		vertices.emplace_back(pos0.x, pos0.y);
		};

	glm::vec2 last_pos(0, 0);
	glm::vec2 pos(0, 0);
	glm::vec2 map_size(size_x, size_y);
	glm::vec2 sections(200, 200);
	for(int y = 0; y < sections.y; y++) {
		pos.y = last_pos.y + (map_size.y / sections.y);
		if(y == sections.y - 1)
			pos.y = map_size.y;

		last_pos.x = 0;
		for(int x = 0; x < sections.x; x++) {
			pos.x = last_pos.x + (map_size.x / sections.x);
			if(x == sections.x - 1)
				pos.x = map_size.x;
			add_quad(land_vertices, last_pos, pos);
			last_pos.x = pos.x;
		}
		last_pos.y = pos.y;
	}

	land_vertex_count = ((uint32_t)land_vertices.size());

	// Create and populate the VBO
	glGenBuffers(1, &land_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, land_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(map_vertex) * land_vertices.size(), &land_vertices[0], GL_STATIC_DRAW);

	// Create and bind the VAO
	glGenVertexArrays(1, &land_vao);
	glBindVertexArray(land_vao);

	// Bind the VBO to 0 of the VAO
	glBindVertexBuffer(0, land_vbo, 0, sizeof(map_vertex));

	// Set up vertex attribute format for the position
	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, offsetof(map_vertex, position_));
	glEnableVertexAttribArray(0);
	glVertexAttribBinding(0, 0);

	glBindVertexArray(0);

	create_border_ogl_objects();
}

display_data::~display_data() {
	if(provinces_texture_handle)
		glDeleteTextures(1, &provinces_texture_handle);
	if(terrain_texture_handle)
		glDeleteTextures(1, &terrain_texture_handle);
	if(terrainsheet_texture_handle)
		glDeleteTextures(1, &terrainsheet_texture_handle);
	if(water_normal)
		glDeleteTextures(1, &water_normal);
	if(colormap_water)
		glDeleteTextures(1, &colormap_water);
	if(colormap_terrain)
		glDeleteTextures(1, &colormap_terrain);
	if(overlay)
		glDeleteTextures(1, &overlay);
	if(province_color)
		glDeleteTextures(1, &province_color);
	if(stripes_texture)
		glDeleteTextures(1, &stripes_texture);
	if(river_body_texture)
		glDeleteTextures(1, &river_body_texture);
	if(national_border_texture)
		glDeleteTextures(1, &national_border_texture);
	if(province_highlight)
		glDeleteTextures(1, &province_highlight);
	if(province_fow)
		glDeleteTextures(1, &province_fow);
	if(unit_arrow_texture)
		glDeleteTextures(1, &unit_arrow_texture);
	if(coastal_border_texture)
		glDeleteTextures(1, &coastal_border_texture);

	if(land_vao)
		glDeleteVertexArrays(1, &land_vao);
	if(river_vao)
		glDeleteVertexArrays(1, &river_vao);
	if(border_vao)
		glDeleteVertexArrays(1, &border_vao);
	if(unit_arrow_vao)
		glDeleteVertexArrays(1, &unit_arrow_vao);
	if(text_line_vao)
		glDeleteVertexArrays(1, &text_line_vao);
	if(drag_box_vao)
		glDeleteVertexArrays(1, &drag_box_vao);
	if(static_mesh_vao)
		glDeleteVertexArrays(1, &static_mesh_vao);

	if(land_vbo)
		glDeleteBuffers(1, &land_vbo);
	if(border_vbo)
		glDeleteBuffers(1, &border_vbo);
	if(river_vbo)
		glDeleteBuffers(1, &river_vbo);
	if(unit_arrow_vbo)
		glDeleteBuffers(1, &unit_arrow_vbo);
	if(text_line_vbo)
		glDeleteBuffers(1, &text_line_vbo);
	if(drag_box_vbo)
		glDeleteBuffers(1, &drag_box_vbo);
	if(coastal_border_vbo)
		glDeleteBuffers(1, &coastal_border_vbo);
	if(static_mesh_vbo)
		glDeleteBuffers(1, &static_mesh_vbo);

	if(terrain_shader)
		glDeleteProgram(terrain_shader);
	if(line_border_shader)
		glDeleteProgram(line_border_shader);
	if(legacy_line_border_shader)
		glDeleteProgram(legacy_line_border_shader);
	if(textured_line_shader)
		glDeleteProgram(textured_line_shader);
	if(legacy_line_river_shader)
		glDeleteProgram(legacy_line_river_shader);
	if(line_unit_arrow_shader)
		glDeleteProgram(line_unit_arrow_shader);
	if(text_line_shader)
		glDeleteProgram(text_line_shader);
	if(drag_box_shader)
		glDeleteProgram(drag_box_shader);
	if(borders_shader)
		glDeleteProgram(borders_shader);
	if(model3d_shader)
		glDeleteProgram(model3d_shader);
}

std::optional<simple_fs::file> try_load_shader(simple_fs::directory& root, native_string_view name) {
	auto shader = simple_fs::open_file(root, name);
	if(!bool(shader))
		ogl::notify_user_of_fatal_opengl_error("Unable to open a necessary shader file");
	return shader;
}

GLuint create_program(simple_fs::file& vshader_file, simple_fs::file& fshader_file) {
	auto vshader_content = simple_fs::view_contents(vshader_file);
	auto vshader_string = std::string_view(vshader_content.data, vshader_content.file_size);
	auto fshader_content = simple_fs::view_contents(fshader_file);
	auto fshader_string = std::string_view(fshader_content.data, fshader_content.file_size);
	return ogl::create_program(vshader_string, fshader_string);
}

void display_data::load_shaders(simple_fs::directory& root) {
	// Map shaders
	auto map_vshader = try_load_shader(root, NATIVE("assets/shaders/map_v.glsl"));
	auto map_fshader = try_load_shader(root, NATIVE("assets/shaders/map_f.glsl"));

	terrain_shader = create_program(*map_vshader, *map_fshader);

	// Line shaders
	auto line_vshader = try_load_shader(root, NATIVE("assets/shaders/line_border_v.glsl"));
	auto line_border_fshader = try_load_shader(root, NATIVE("assets/shaders/line_border_f.glsl"));

	auto line_unit_arrow_vshader = try_load_shader(root, NATIVE("assets/shaders/line_unit_arrow_v.glsl"));
	auto line_unit_arrow_fshader = try_load_shader(root, NATIVE("assets/shaders/line_unit_arrow_f.glsl"));

	auto text_line_vshader = try_load_shader(root, NATIVE("assets/shaders/text_line_v.glsl"));
	auto text_line_fshader = try_load_shader(root, NATIVE("assets/shaders/text_line_f.glsl"));

	auto screen_vshader = try_load_shader(root, NATIVE("assets/shaders/screen_v.glsl"));
	auto black_color_fshader = try_load_shader(root, NATIVE("assets/shaders/black_color_f.glsl"));
	auto white_color_fshader = try_load_shader(root, NATIVE("assets/shaders/white_color_f.glsl"));
	legacy_line_border_shader = create_program(*line_vshader, *black_color_fshader);
	
	auto tline_vshader = try_load_shader(root, NATIVE("assets/shaders/textured_line_v.glsl"));
	auto tline_fshader = try_load_shader(root, NATIVE("assets/shaders/textured_line_f.glsl"));
	textured_line_shader = create_program(*tline_vshader, *tline_fshader);

	auto tlineb_vshader = try_load_shader(root, NATIVE("assets/shaders/textured_line_b_v.glsl"));
	auto tlineb_fshader = try_load_shader(root, NATIVE("assets/shaders/textured_line_b_f.glsl"));
	borders_shader = create_program(*tlineb_vshader, *tlineb_fshader);

	auto model3d_vshader = try_load_shader(root, NATIVE("assets/shaders/model3d_v.glsl"));
	auto model3d_fshader = try_load_shader(root, NATIVE("assets/shaders/model3d_f.glsl"));
	model3d_shader = create_program(*model3d_vshader, *model3d_fshader);

	line_unit_arrow_shader = create_program(*line_unit_arrow_vshader, *line_unit_arrow_fshader);
	text_line_shader = create_program(*text_line_vshader, *text_line_fshader);
	drag_box_shader = create_program(*screen_vshader, *white_color_fshader);
}

void display_data::render(sys::state& state, glm::vec2 screen_size, glm::vec2 offset, float zoom, map_view map_view_mode, map_mode::mode active_map_mode, glm::mat3 globe_rotation, float time_counter) {
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	if(ogl::msaa_enabled(state)) {
		glBindFramebuffer(GL_FRAMEBUFFER, state.open_gl.msaa_framebuffer);
		glClearColor(1.f, 1.f, 1.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, provinces_texture_handle);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, terrain_texture_handle);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D_ARRAY, terrainsheet_texture_handle);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, water_normal);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, colormap_water);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, colormap_terrain);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, overlay);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D_ARRAY, province_color);
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, colormap_political);
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, province_highlight);
	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, stripes_texture);
	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_2D, unit_arrow_texture);
	glActiveTexture(GL_TEXTURE13);
	glBindTexture(GL_TEXTURE_2D, province_fow);
	glActiveTexture(GL_TEXTURE14);
	glBindTexture(GL_TEXTURE_2D, river_body_texture);
	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_2D, national_border_texture);
	//glActiveTexture(GL_TEXTURE16);
	//glBindTexture(GL_TEXTURE_2D, national_border_texture);

	// Load general shader stuff, used by both land and borders
	auto load_shader = [&](GLuint program) {
		glUseProgram(program);

		// uniform vec2 offset
		glUniform2f(0, offset.x + 0.f, offset.y);
		// uniform float aspect_ratio
		glUniform1f(1, screen_size.x / screen_size.y);
		// uniform float zoom
		glUniform1f(2, zoom);
		// uniform vec2 map_size
		glUniform2f(3, GLfloat(size_x), GLfloat(size_y));
		glUniformMatrix3fv(5, 1, GL_FALSE, glm::value_ptr(glm::mat3(globe_rotation)));
		glUniform1f(11, state.user_settings.gamma);

		GLuint vertex_subroutines;
		// calc_gl_position()
		if(map_view_mode == map_view::globe)
			vertex_subroutines = 0; // globe_coords()
		else
			vertex_subroutines = 1; // flat_coords()
		glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &vertex_subroutines);
		};

	load_shader(terrain_shader);
	{ // Land specific shader uniform
		glUniform1f(4, time_counter);
		// get_land()
		GLuint fragment_subroutines[2];
		if(active_map_mode == map_mode::mode::terrain)
			fragment_subroutines[0] = 0; // get_land_terrain()
		else if(zoom > 5)
			fragment_subroutines[0] = 1; // get_land_political_close()
		else
			fragment_subroutines[0] = 2; // get_land_political_far()
		// get_water()
		if(active_map_mode == map_mode::mode::terrain || zoom > 5)
			fragment_subroutines[1] = 3; // get_water_terrain()
		else
			fragment_subroutines[1] = 4; // get_water_political()
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 2, fragment_subroutines);
	}
	glBindVertexArray(land_vao);
	glDrawArrays(GL_TRIANGLES, 0, land_vertex_count);

	constexpr float thickness_sizes[] = {
		0.00085f,// * 2.f,
		0.00055f,// * 2.f,
		0.00033f,// * 2.f
	};

	// Draw the rivers

	load_shader(textured_line_shader);
	glUniform1f(6, time_counter);
	glUniform1f(4, 0.00008f);

	glBindVertexArray(river_vao);
	glBindBuffer(GL_ARRAY_BUFFER, river_vbo);

	//glMultiDrawArrays(GL_TRIANGLE_STRIP, river_starts.data(), river_counts.data(), GLsizei(river_starts.size()));

	// Default border parameters
	constexpr float border_type_national = 0.f;
	constexpr float border_type_provincial = 1.f;
	constexpr float border_type_regional = 2.f;
	constexpr float border_type_coastal = 3.f;

	load_shader(legacy_line_border_shader);

	glBindVertexArray(border_vao);
	glBindBuffer(GL_ARRAY_BUFFER, border_vbo);
	if(zoom > 8) { // Render all borders
		std::vector<GLint> first;
		std::vector<GLsizei> count;
		glUniform1f(4, thickness_sizes[2]);
		glUniform1f(12, border_type_provincial);
		for(auto& border : borders) {
			if((border.type_flag & (province::border::state_bit | province::border::national_bit | province::border::coastal_bit)) == 0) {
				first.push_back(border.start_index);
				count.push_back(border.count);
			}
		}
		glMultiDrawArrays(GL_TRIANGLES, first.data(), count.data(), GLsizei(count.size()));
		first.clear();
		count.clear();
		glUniform1f(4, thickness_sizes[1]);
		glUniform1f(12, border_type_regional);
		for(auto& border : borders) {
			if((border.type_flag & (province::border::state_bit)) != 0 && (border.type_flag & (province::border::coastal_bit)) == 0) {
				first.push_back(border.start_index);
				count.push_back(border.count);
			}
		}
		glMultiDrawArrays(GL_TRIANGLES, first.data(), count.data(), GLsizei(count.size()));
		first.clear();
		count.clear();
		glUniform1f(4, thickness_sizes[0]);
		glUniform1f(12, border_type_national);
		for(auto& border : borders) {
			if((border.type_flag & (province::border::national_bit)) != 0 && (border.type_flag & (province::border::coastal_bit)) == 0) {
				first.push_back(border.start_index);
				count.push_back(border.count);
			}
		}
		glMultiDrawArrays(GL_TRIANGLES, first.data(), count.data(), GLsizei(count.size()));
		first.clear();
		count.clear();
		glUniform1f(4, thickness_sizes[0]);
		glUniform1f(12, border_type_coastal);
		for(auto& border : borders) {
			if((border.type_flag & (province::border::coastal_bit)) != 0) {
				first.push_back(border.start_index);
				count.push_back(border.count);
			}
		}
		//glMultiDrawArrays(GL_TRIANGLES, first.data(), count.data(), GLsizei(count.size()));
	} else if(zoom > 5) { // Render state borders also
		std::vector<GLint> first;
		std::vector<GLsizei> count;
		glUniform1f(4, thickness_sizes[1]);
		glUniform1f(12, border_type_regional);
		for(auto& border : borders) {
			if((border.type_flag & (province::border::state_bit)) != 0 && (border.type_flag & (province::border::coastal_bit)) == 0) {
				first.push_back(border.start_index);
				count.push_back(border.count);
			}
		}
		glMultiDrawArrays(GL_TRIANGLES, first.data(), count.data(), GLsizei(count.size()));
		first.clear();
		count.clear();
		glUniform1f(4, thickness_sizes[0]);
		glUniform1f(12, border_type_national);
		for(auto& border : borders) {
			if((border.type_flag & (province::border::national_bit)) != 0 && (border.type_flag & (province::border::coastal_bit)) == 0) {
				first.push_back(border.start_index);
				count.push_back(border.count);
			}
		}
		glMultiDrawArrays(GL_TRIANGLES, first.data(), count.data(), GLsizei(count.size()));
		first.clear();
		count.clear();
		glUniform1f(4, thickness_sizes[0]);
		glUniform1f(12, border_type_coastal);
		for(auto& border : borders) {
			if((border.type_flag & (province::border::coastal_bit)) != 0) {
				first.push_back(border.start_index);
				count.push_back(border.count);
			}
		}
		//glMultiDrawArrays(GL_TRIANGLES, first.data(), count.data(), GLsizei(count.size()));
	} else {
		std::vector<GLint> first;
		std::vector<GLsizei> count;
		glUniform1f(4, thickness_sizes[1]);
		glUniform1f(12, border_type_provincial);
		for(auto& border : borders) {
			if((border.type_flag & (province::border::national_bit)) != 0 && (border.type_flag & (province::border::coastal_bit)) == 0) {
				first.push_back(border.start_index);
				count.push_back(border.count);
			}
		}
		glMultiDrawArrays(GL_TRIANGLES, first.data(), count.data(), GLsizei(count.size()));
		first.clear();
		count.clear();
		glUniform1f(4, thickness_sizes[0]);
		glUniform1f(12, border_type_regional);
		for(auto& border : borders) {
			if((border.type_flag & (province::border::coastal_bit)) != 0) {
				first.push_back(border.start_index);
				count.push_back(border.count);
			}
		}
		//glMultiDrawArrays(GL_TRIANGLES, first.data(), count.data(), GLsizei(count.size()));
	}

	// coasts
	{
		if(map_view_mode == map_view::globe) {
			glDisable(GL_CULL_FACE);
		}

		glUseProgram(borders_shader);
		glUniform2f(0, offset.x + 0.f, offset.y);
		glUniform1f(1, screen_size.x / screen_size.y);
		glUniform1f(2, zoom);
		glUniform2f(3, GLfloat(size_x), GLfloat(size_y));
		glUniformMatrix3fv(5, 1, GL_FALSE, glm::value_ptr(glm::mat3(globe_rotation)));
		glUniform1f(11, state.user_settings.gamma);

		{
			GLuint vertex_subroutines[2] = {};
			if(map_view_mode == map_view::globe) {
				vertex_subroutines[0] = 0; // globe_coords()
				vertex_subroutines[1] = 2; // globe_coords()
			} else {
				vertex_subroutines[0] = 1; // flat_coords()
				vertex_subroutines[1] = 3; // globe_coords()
			}
			glUniformSubroutinesuiv(GL_VERTEX_SHADER, 2, vertex_subroutines);
		}

		glUniform1f(4, 0.0002f); // width

		glActiveTexture(GL_TEXTURE14);
		glBindTexture(GL_TEXTURE_2D, coastal_border_texture);

		glBindVertexArray(river_vao);
		glBindBuffer(GL_ARRAY_BUFFER, coastal_border_vbo);

		glMultiDrawArrays(GL_TRIANGLE_STRIP, coastal_starts.data(), coastal_counts.data(), GLsizei(coastal_starts.size()));

		if(map_view_mode == map_view::globe) {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}
	}

	/*
	{
		load_shader(model3d_shader);
		glUniform1f(6, time_counter);
		glBindVertexArray(static_mesh_vao);
		glBindBuffer(GL_ARRAY_BUFFER, static_mesh_vbo);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glMultiDrawArrays(GL_TRIANGLES, static_mesh_starts.data(), static_mesh_counts.data(), GLsizei(static_mesh_starts.size()));
	}
	*/

	if(!unit_arrow_vertices.empty()) {
		// Draw the unit arrows
		load_shader(line_unit_arrow_shader);
		glUniform1f(4, 0.005f);
		glBindVertexArray(unit_arrow_vao);
		glBindBuffer(GL_ARRAY_BUFFER, unit_arrow_vbo);
		glMultiDrawArrays(GL_TRIANGLE_STRIP, unit_arrow_starts.data(), unit_arrow_counts.data(), (GLsizei)unit_arrow_counts.size());
	}

	if(!drag_box_vertices.empty()) {
		glUseProgram(drag_box_shader);
		glUniform1f(11, state.user_settings.gamma);
		glBindVertexArray(drag_box_vao);
		glBindBuffer(GL_ARRAY_BUFFER, drag_box_vbo);
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)drag_box_vertices.size());
	}

	if(state.user_settings.map_label != sys::map_label_mode::none && zoom < 5 && !text_line_vertices.empty()) {
		load_shader(text_line_shader);
		glUniform1f(12, state.font_collection.map_font_is_black ? 1.f : 0.f);
		auto const& f = state.font_collection.fonts[2];
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, f.textures[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, f.textures[1]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, f.textures[2]);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, f.textures[3]);
		glBindVertexArray(text_line_vao);
		glBindBuffer(GL_ARRAY_BUFFER, text_line_vbo);
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)text_line_vertices.size());
	}

	glBindVertexArray(0);
	glDisable(GL_CULL_FACE);

	if(ogl::msaa_enabled(state)) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, state.open_gl.msaa_framebuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, state.open_gl.msaa_interbuffer);
		glBlitFramebuffer(0, 0, GLint(screen_size.x), GLint(screen_size.y), 0, 0, GLint(screen_size.x), GLint(screen_size.y), GL_COLOR_BUFFER_BIT, GL_NEAREST);
		// 3. now render quad with scene's visuals as its texture image
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// draw Screen quad
		glUseProgram(state.open_gl.msaa_shader_program);
		glUniform1f(0, state.user_settings.gaussianblur_level);
		glUniform2f(1, screen_size.x, screen_size.y);
		//
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, state.open_gl.msaa_texture); // use the now resolved color attachment as the quad's texture
		glBindVertexArray(state.open_gl.msaa_vao);
		//glBindBuffer(GL_ARRAY_BUFFER, state.open_gl.msaa_vbo);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}

GLuint load_province_map(std::vector<uint16_t>& province_index, uint32_t size_x, uint32_t size_y) {
	GLuint texture_handle;
	glGenTextures(1, &texture_handle);
	if(texture_handle) {
		glBindTexture(GL_TEXTURE_2D, texture_handle);

		// Create a texture with only one byte color
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG8, size_x, size_y);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size_x, size_y, GL_RG, GL_UNSIGNED_BYTE, &province_index[0]);
		glBindTexture(GL_TEXTURE_2D, 0);
		set_gltex_parameters(texture_handle, GL_TEXTURE_2D, GL_NEAREST, GL_CLAMP_TO_EDGE);
	}
	return texture_handle;
}

void display_data::gen_prov_color_texture(GLuint texture_handle, std::vector<uint32_t> const& prov_color, uint8_t layers) {
	if(layers == 1) {
		glBindTexture(GL_TEXTURE_2D, texture_handle);
	} else {
		glBindTexture(GL_TEXTURE_2D_ARRAY, texture_handle);
	}
	uint32_t rows = ((uint32_t)prov_color.size()) / 256;
	uint32_t left_on_last_row = ((uint32_t)prov_color.size()) % 256;

	uint32_t x = 0;
	uint32_t y = 0;
	uint32_t width = 256;
	uint32_t height = rows;

	if(layers == 1) {
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, &prov_color[0]);
	} else {
		// Set the texture data for each layer
		for(int i = 0; i < layers; i++) {
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, x, y, i, width, height / layers, 1, GL_RGBA, GL_UNSIGNED_BYTE, &prov_color[i * (prov_color.size() / layers)]);
		}
	}

	x = 0;
	y = rows;
	width = left_on_last_row;
	height = 1;

	// SCHOMBERT: added a conditional to block reading from after the end in the case it is evenly divisible by 256
	// SCHOMBERT: that looks right to me, but I don't fully understand the intent
	if(left_on_last_row > 0 && layers == 1)
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, &prov_color[rows * 256]);

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void display_data::set_selected_province(sys::state& state, dcon::province_id prov_id) {
	std::vector<uint32_t> province_highlights(state.world.province_size() + 1);
	if(prov_id) {
		province_highlights[province::to_map_id(prov_id)] = 0x2B2B2B2B;
	}
	gen_prov_color_texture(province_highlight, province_highlights);
}

void display_data::set_province_color(std::vector<uint32_t> const& prov_color) {
	gen_prov_color_texture(province_color, prov_color, 2);
}

void add_drag_box_line(std::vector<screen_vertex>& drag_box_vertices, glm::vec2 pos1, glm::vec2 pos2, glm::vec2 size, bool vertical) {
	if(vertical) {
		pos1.y -= size.y;
		pos2.y += size.y;
		size.y = 0;
	} else {
		pos1.x -= size.x;
		pos2.x += size.x;
		size.x = 0;
	}
	drag_box_vertices.emplace_back(pos1.x + size.x, pos1.y - size.y);
	drag_box_vertices.emplace_back(pos1.x - size.x, pos1.y + size.y);
	drag_box_vertices.emplace_back(pos2.x - size.x, pos2.y + size.y);

	drag_box_vertices.emplace_back(pos2.x - size.x, pos2.y + size.y);
	drag_box_vertices.emplace_back(pos2.x + size.x, pos2.y - size.y);
	drag_box_vertices.emplace_back(pos1.x + size.x, pos1.y - size.y);
}

void display_data::set_drag_box(bool draw_box, glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pixel_size) {
	drag_box_vertices.clear();
	if(!draw_box)
		return;

	if(pos1.x > pos2.x)
		std::swap(pos1.x, pos2.x);
	if(pos1.y > pos2.y)
		std::swap(pos1.y, pos2.y);

	glm::vec2 size = pixel_size;
	// Vertical lines
	add_drag_box_line(drag_box_vertices, { pos1.x, pos1.y }, { pos1.x, pos2.y }, size, true);
	add_drag_box_line(drag_box_vertices, { pos2.x, pos1.y }, { pos2.x, pos2.y }, size, true);

	// Horizontal lines
	add_drag_box_line(drag_box_vertices, { pos1.x, pos1.y }, { pos2.x, pos1.y }, size, false);
	add_drag_box_line(drag_box_vertices, { pos1.x, pos2.y }, { pos2.x, pos2.y }, size, false);

	glBindBuffer(GL_ARRAY_BUFFER, drag_box_vbo);
	if(drag_box_vertices.size() > 0) {
		glBufferData(GL_ARRAY_BUFFER, sizeof(screen_vertex) * drag_box_vertices.size(), &drag_box_vertices[0], GL_STATIC_DRAW);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void add_arrow_to_buffer(std::vector<map::curved_line_vertex>& buffer, glm::vec2 start, glm::vec2 end, glm::vec2 prev_normal_dir, glm::vec2 next_normal_dir, float fill_progress, bool end_arrow, float size_x, float size_y) {
	constexpr float type_filled = 2.f;
	constexpr float type_unfilled = 0.f;
	constexpr float type_end = 3.f;
	glm::vec2 curr_dir = normalize(end - start);
	start /= glm::vec2(size_x, size_y);
	end /= glm::vec2(size_x, size_y);

	if(fill_progress != 0) {
		if(fill_progress < 1.0f) {
			auto pos3 = glm::mix(start, end, fill_progress);
			auto midd_normal_dir = glm::vec2(-curr_dir.y, curr_dir.x);

			buffer.emplace_back(pos3, +midd_normal_dir, -curr_dir, glm::vec2(1.0f, 0.0f), type_filled);//C
			buffer.emplace_back(pos3, -midd_normal_dir, -curr_dir, glm::vec2(1.0f, 1.0f), type_filled);//D
			buffer.emplace_back(pos3, +midd_normal_dir, -curr_dir, glm::vec2(1.0f, 0.0f), type_unfilled);//C
			buffer.emplace_back(pos3, -midd_normal_dir, -curr_dir, glm::vec2(1.0f, 1.0f), type_unfilled);//D

			// Not filled - transition from "filled" to "unfilled"
			buffer.emplace_back(end, +next_normal_dir, -curr_dir, glm::vec2(1.0f, 0.0f), type_unfilled);//C
			buffer.emplace_back(end, -next_normal_dir, -curr_dir, glm::vec2(1.0f, 1.0f), type_unfilled);//D
		} else {
			// Not filled - transition from "filled" to "unfilled"
			buffer.emplace_back(end, +next_normal_dir, -curr_dir, glm::vec2(1.0f, 0.0f), type_filled);//C
			buffer.emplace_back(end, -next_normal_dir, -curr_dir, glm::vec2(1.0f, 1.0f), type_filled);//D
		}
	} else {
		// All unfilled!
		if(buffer.back().type_ == type_filled) {
			buffer.emplace_back(buffer[buffer.size() - 2]);//C
			buffer.emplace_back(buffer[buffer.size() - 2]);//D
			buffer[buffer.size() - 2].type_ = type_unfilled;
			buffer[buffer.size() - 1].type_ = type_unfilled;
		}
		buffer.emplace_back(end, +next_normal_dir, -curr_dir, glm::vec2(1.0f, 0.0f), type_unfilled);//C
		buffer.emplace_back(end, -next_normal_dir, -curr_dir, glm::vec2(1.0f, 1.0f), type_unfilled);//D
	}
	if(end_arrow) {
		if(buffer.back().type_ == type_filled) {
			buffer.emplace_back(buffer[buffer.size() - 2]);//C
			buffer.emplace_back(buffer[buffer.size() - 2]);//D
			buffer[buffer.size() - 2].type_ = type_unfilled;
			buffer[buffer.size() - 1].type_ = type_unfilled;
		}
		buffer.emplace_back(end, +next_normal_dir, -curr_dir, glm::vec2(1.0f, 0.0f), type_end);//C
		buffer.emplace_back(end, -next_normal_dir, -curr_dir, glm::vec2(1.0f, 1.0f), type_end);//D
	}
}

constexpr inline uint32_t default_num_b_segments = 16;
constexpr inline float control_point_length_factor = 0.3f;

void add_bezier_to_buffer(std::vector<map::curved_line_vertex>& buffer, glm::vec2 start, glm::vec2 end, glm::vec2 start_per, glm::vec2 end_per, float progress, bool last_curve, float size_x, float size_y, uint32_t num_b_segments) {
	auto control_point_length = glm::length(end - start) * control_point_length_factor;

	auto start_control_point = start_per * control_point_length + start;
	auto end_control_point = end_per * control_point_length + end;

	auto bpoint = [=](float t) {
		auto u = 1.0f - t;
		return 0.0f
			+ (u * u * u) * start
			+ (3.0f * u * u * t) * start_control_point
			+ (3.0f * u * t * t) * end_control_point
			+ (t * t * t) * end;
		};

	auto last_normal = glm::vec2(-start_per.y, start_per.x);
	glm::vec2 next_normal{ 0.0f, 0.0f };

	for(uint32_t i = 0; i < num_b_segments - 1; ++i) {
		auto t_start = float(i) / float(num_b_segments);
		auto t_end = float(i + 1) / float(num_b_segments);
		auto t_next = float(i + 2) / float(num_b_segments);

		auto start_point = bpoint(t_start);
		auto end_point = bpoint(t_end);
		auto next_point = bpoint(t_next);

		next_normal = glm::normalize(end_point - start_point) + glm::normalize(end_point - next_point);
		auto temp = glm::normalize(end_point - start_point);
		if(glm::length(next_normal) < 0.00001f) {
			next_normal = glm::normalize(glm::vec2(-temp.y, temp.x));
		} else {
			next_normal = glm::normalize(next_normal);
			if(glm::dot(glm::vec2(-temp.y, temp.x), next_normal) < 0) {
				next_normal = -next_normal;
			}
		}

		if(progress > 0.0f) {
			if(t_end <= progress) { // filled
				add_arrow_to_buffer(buffer, start_point, end_point, last_normal, next_normal, 1.0f, false, size_x, size_y);
			} else if(progress < t_start) { // empty
				add_arrow_to_buffer(buffer, start_point, end_point, last_normal, next_normal, 0.0f, false, size_x, size_y);
			} else {
				auto effective_progress = (progress - t_start) * float(num_b_segments);
				add_arrow_to_buffer(buffer, start_point, end_point, last_normal, next_normal, effective_progress, false, size_x, size_y);
			}
		} else {
			add_arrow_to_buffer(buffer, start_point, end_point, last_normal, next_normal, 0.0f, false, size_x, size_y);
		}

		last_normal = next_normal;
	}
	{
		next_normal = glm::vec2(end_per.y, -end_per.x);
		auto t_start = float(num_b_segments - 1) / float(num_b_segments);
		auto t_end = 1.0f;
		auto start_point = bpoint(t_start);
		auto end_point = bpoint(t_end);

		if(progress > 0.0f) {
			if(t_end <= progress) { // filled
				add_arrow_to_buffer(buffer, start_point, end_point, last_normal, next_normal, 1.0f, last_curve, size_x, size_y);
			} else if(progress < t_start) { // empty
				add_arrow_to_buffer(buffer, start_point, end_point, last_normal, next_normal, 0.0f, last_curve, size_x, size_y);
			} else {
				auto effective_progress = (progress - t_start) * float(num_b_segments);
				add_arrow_to_buffer(buffer, start_point, end_point, last_normal, next_normal, effective_progress, last_curve, size_x, size_y);
			}
		} else {
			add_arrow_to_buffer(buffer, start_point, end_point, last_normal, next_normal, 0.0f, last_curve, size_x, size_y);
		}
	}
}

void add_tl_segment_buffer(std::vector<map::textured_line_vertex>& buffer, glm::vec2 start, glm::vec2 end, glm::vec2 next_normal_dir, float size_x, float size_y, float& distance) {
	start /= glm::vec2(size_x, size_y);
	end /= glm::vec2(size_x, size_y);

	distance += glm::distance(start, end);
	buffer.emplace_back(textured_line_vertex{ end, +next_normal_dir, 0.0f, distance });//C
	buffer.emplace_back(textured_line_vertex{ end, -next_normal_dir, 1.0f, distance });//D
}

void add_tl_bezier_to_buffer(std::vector<map::textured_line_vertex>& buffer, glm::vec2 start, glm::vec2 end, glm::vec2 start_per, glm::vec2 end_per, float progress, bool last_curve, float size_x, float size_y, uint32_t num_b_segments, float& distance) {
	auto control_point_length = glm::length(end - start) * control_point_length_factor;

	auto start_control_point = start_per * control_point_length + start;
	auto end_control_point = end_per * control_point_length + end;

	auto bpoint = [=](float t) {
		auto u = 1.0f - t;
		return 0.0f
			+ (u * u * u) * start
			+ (3.0f * u * u * t) * start_control_point
			+ (3.0f * u * t * t) * end_control_point
			+ (t * t * t) * end;
		};

	auto last_normal = glm::vec2(-start_per.y, start_per.x);
	glm::vec2 next_normal{ 0.0f, 0.0f };

	for(uint32_t i = 0; i < num_b_segments - 1; ++i) {
		auto t_start = float(i) / float(num_b_segments);
		auto t_end = float(i + 1) / float(num_b_segments);
		auto t_next = float(i + 2) / float(num_b_segments);

		auto start_point = bpoint(t_start);
		auto end_point = bpoint(t_end);
		auto next_point = bpoint(t_next);

		next_normal = glm::normalize(end_point - start_point) + glm::normalize(end_point - next_point);
		auto temp = glm::normalize(end_point - start_point);
		if(glm::length(next_normal) < 0.00001f) {
			next_normal = glm::normalize(glm::vec2(-temp.y, temp.x));
		} else {
			next_normal = glm::normalize(next_normal);
			if(glm::dot(glm::vec2(-temp.y, temp.x), next_normal) < 0) {
				next_normal = -next_normal;
			}
		}

		add_tl_segment_buffer(buffer, start_point, end_point, next_normal, size_x, size_y, distance);

		last_normal = next_normal;
	}
	{
		next_normal = glm::vec2(end_per.y, -end_per.x);
		auto t_start = float(num_b_segments - 1) / float(num_b_segments);
		auto t_end = 1.0f;
		auto start_point = bpoint(t_start);
		auto end_point = bpoint(t_end);

		
		add_tl_segment_buffer(buffer, start_point, end_point, next_normal, size_x, size_y, distance);
	}
}

namespace duplicates {
glm::vec2 get_port_location(sys::state& state, dcon::province_id p) {
	auto pt = state.world.province_get_port_to(p);
	if(!pt)
		return glm::vec2{};

	auto adj = state.world.get_province_adjacency_by_province_pair(p, pt);
	assert(adj);
	auto id = adj.index();
	auto& map_data = state.map_state.map_data;
	auto& border = map_data.borders[id];
	auto& vertex = map_data.border_vertices[border.start_index + border.count / 2];
	glm::vec2 map_size = glm::vec2(map_data.size_x, map_data.size_y);

	return vertex.position_ * map_size;
}

bool is_sea_province(sys::state& state, dcon::province_id prov_id) {
	return prov_id.index() >= state.province_definitions.first_sea_province.index();
}

glm::vec2 get_navy_location(sys::state& state, dcon::province_id prov_id) {
	if(is_sea_province(state, prov_id))
		return state.world.province_get_mid_point(prov_id);
	else
		return get_port_location(state, prov_id);
}

glm::vec2 get_army_location(sys::state& state, dcon::province_id prov_id) {
	return state.world.province_get_mid_point(prov_id);
}
}

glm::vec2 put_in_local(glm::vec2 new_point, glm::vec2 base_point, float size_x) {
	auto uadjx = std::abs(new_point.x - base_point.x);
	auto ladjx = std::abs(new_point.x - size_x - base_point.x);
	auto radjx = std::abs(new_point.x + size_x - base_point.x);
	if(uadjx < ladjx) {
		return uadjx < radjx ? new_point : glm::vec2{ new_point.x + size_x, new_point.y };
	} else {
		return ladjx < radjx ? glm::vec2{ new_point.x - size_x, new_point.y } : glm::vec2{ new_point.x + size_x, new_point.y };
	}
}

void make_navy_path(sys::state& state, std::vector<map::curved_line_vertex>& buffer, dcon::navy_id selected_navy, float size_x, float size_y) {
	auto path = state.world.navy_get_path(selected_navy);
	if(auto ps = path.size(); ps > 0) {
		auto progress = military::fractional_distance_covered(state, selected_navy);

		glm::vec2 current_pos = duplicates::get_navy_location(state, state.world.navy_get_location_from_navy_location(selected_navy));
		glm::vec2 next_pos = put_in_local(duplicates::get_navy_location(state, path[ps - 1]), current_pos, size_x);
		glm::vec2 prev_perpendicular = glm::normalize(next_pos - current_pos);

		auto start_normal = glm::vec2(-prev_perpendicular.y, prev_perpendicular.x);
		auto norm_pos = current_pos / glm::vec2(size_x, size_y);

		buffer.emplace_back(norm_pos, +start_normal, glm::vec2{ 0,0 }, glm::vec2(0.0f, 0.0f), progress > 0.0f ? 2.0f : 0.0f);
		buffer.emplace_back(norm_pos, -start_normal, glm::vec2{ 0,0 }, glm::vec2(0.0f, 1.0f), progress > 0.0f ? 2.0f : 0.0f);

		for(auto i = ps; i-- > 0;) {
			glm::vec2 next_perpendicular{ 0.0f, 0.0f };
			next_pos = put_in_local(duplicates::get_navy_location(state, path[i]), current_pos, size_x);

			if(i > 0) {
				glm::vec2 next_next_pos = put_in_local(duplicates::get_navy_location(state, path[i - 1]), next_pos, size_x);
				glm::vec2 a_per = glm::normalize(next_pos - current_pos);
				glm::vec2 b_per = glm::normalize(next_pos - next_next_pos);
				glm::vec2 temp = a_per + b_per;
				if(glm::length(temp) < 0.00001f) {
					next_perpendicular = -a_per;
				} else {
					next_perpendicular = glm::normalize(glm::vec2{ -temp.y, temp.x });

					if(glm::dot(a_per, -next_perpendicular) < glm::dot(a_per, next_perpendicular)) {
						next_perpendicular *= -1.0f;
					}
				}
			} else {
				next_perpendicular = glm::normalize(current_pos - next_pos);
			}

			add_bezier_to_buffer(buffer, current_pos, next_pos, prev_perpendicular, next_perpendicular, i == ps - 1 ? progress : 0.0f, i == 0, size_x, size_y, default_num_b_segments);

			prev_perpendicular = -1.0f * next_perpendicular;
			current_pos = duplicates::get_navy_location(state, path[i]);
		}
	}
}


void make_army_path(sys::state& state, std::vector<map::curved_line_vertex>& buffer, dcon::army_id selected_army, float size_x, float size_y) {
	auto path = state.world.army_get_path(selected_army);
	if(auto ps = path.size(); ps > 0) {
		auto progress = military::fractional_distance_covered(state, selected_army);

		glm::vec2 current_pos = duplicates::get_army_location(state, state.world.army_get_location_from_army_location(selected_army));
		glm::vec2 next_pos = put_in_local(duplicates::get_army_location(state, path[ps - 1]), current_pos, size_x);
		glm::vec2 prev_perpendicular = glm::normalize(next_pos - current_pos);


		auto start_normal = glm::vec2(-prev_perpendicular.y, prev_perpendicular.x);
		auto norm_pos = current_pos / glm::vec2(size_x, size_y);

		buffer.emplace_back(norm_pos, +start_normal, glm::vec2{0,0}, glm::vec2(0.0f, 0.0f), progress > 0.0f ? 2.0f : 0.0f);
		buffer.emplace_back(norm_pos, -start_normal, glm::vec2{ 0,0 }, glm::vec2(0.0f, 1.0f), progress > 0.0f ? 2.0f : 0.0f);

		for(auto i = ps; i-- > 0;) {
			glm::vec2 next_perpendicular{ 0.0f, 0.0f };
			next_pos = put_in_local(duplicates::get_army_location(state, path[i]), current_pos, size_x);

			if(i > 0) {
				glm::vec2 next_next_pos = put_in_local(duplicates::get_army_location(state, path[i - 1]), next_pos, size_x);
				glm::vec2 a_per = glm::normalize(next_pos - current_pos);
				glm::vec2 b_per = glm::normalize(next_pos - next_next_pos);
				glm::vec2 temp = a_per + b_per;
				if(glm::length(temp) < 0.00001f) {
					next_perpendicular = -a_per;
				} else {
					next_perpendicular = glm::normalize(glm::vec2{ -temp.y, temp.x });

					if(glm::dot(a_per, -next_perpendicular) < glm::dot(a_per, next_perpendicular)) {
						next_perpendicular *= -1.0f;
					}
				}
			} else {
				next_perpendicular = glm::normalize(current_pos - next_pos);
			}

			add_bezier_to_buffer(buffer, current_pos, next_pos, prev_perpendicular, next_perpendicular, i == ps - 1 ? progress : 0.0f, i == 0, size_x, size_y, default_num_b_segments);

			prev_perpendicular = -1.0f * next_perpendicular;
			current_pos = duplicates::get_army_location(state, path[i]);
		}
	}
}

void display_data::set_unit_arrows(std::vector<std::vector<glm::vec2>> const& arrows, std::vector<float> const& progresses) {
	// TODO: Remove this unused function
}

void display_data::set_text_lines(sys::state& state, std::vector<text_line_generator_data> const& data) {
	text_line_vertices.clear();

	const auto map_x_scaling = float(size_x) / float(size_y);

	for(const auto& e : data) {
		// omit invalid, nan or infinite coefficients
		if(!std::isfinite(e.coeff[0]) || !std::isfinite(e.coeff[1]) || !std::isfinite(e.coeff[2]) || !std::isfinite(e.coeff[3]))
			continue;

		auto effective_ratio = e.ratio.x * map_x_scaling / e.ratio.y;

		auto& f = state.font_collection.fonts[2];
		if(!f.loaded)
			return;

		float text_length = f.text_extent(state, e.text.data(), uint32_t(e.text.length()), 1);
		assert(std::isfinite(text_length) && text_length != 0.f);
		// y = a + bx + cx^2 + dx^3
		// y = mo[0] + mo[1] * x + mo[2] * x * x + mo[3] * x * x * x
		auto poly_fn = [&](float x) {
			return e.coeff[0] + e.coeff[1] * x + e.coeff[2] * x * x + e.coeff[3] * x * x * x;
			};
		float x_step = (1.f / float(e.text.length() * 32.f));
		float curve_length = 0.f; //width of whole string polynomial
		for(float x = 0.f; x <= 1.f; x += x_step)
			curve_length += 2.0f * glm::length(glm::vec2(x_step * e.ratio.x, (poly_fn(x) - poly_fn(x + x_step)) * e.ratio.y));

		float size = (curve_length / text_length) * 0.85f;
		if(size > 200.0f) {
			size = 200.0f + (size - 200.0f) * 0.5f;
		}

		auto real_text_size = size / (size_x * 2.0f);

		float margin = (curve_length - text_length * size) / 2.0f;

		float x = 0.f;

		for(float accumulated_length = 0.f; ; x += x_step) {
			auto added_distance = 2.0f * glm::length(glm::vec2(x_step * e.ratio.x, (poly_fn(x) - poly_fn(x + x_step)) * e.ratio.y));
			if(accumulated_length + added_distance >= margin) {
				x += x_step * (margin - accumulated_length) / added_distance;
				break;
			}
			accumulated_length += added_distance;
		}

		for(int32_t i = 0; i < int32_t(e.text.length()); i++) {
			if(e.text[i] != ' ') { // skip spaces, only leaving a , well, space!
				// Add up baseline and kerning offsets
				auto dpoly_fn = [&](float x) {
					// y = a + 1bx^1 + 1cx^2 + 1dx^3
					// y = 0 + 1bx^0 + 2cx^1 + 3dx^2
					return e.coeff[1] + 2.f * e.coeff[2] * x + 3.f * e.coeff[3] * x * x;
					};
				glm::vec2 glyph_positions{ f.glyph_positions[uint8_t(e.text[i])].x / 64.f, -f.glyph_positions[uint8_t(e.text[i])].y / 64.f };

				glm::vec2 curr_dir = glm::normalize(glm::vec2(effective_ratio, dpoly_fn(x)));
				glm::vec2 curr_normal_dir = glm::vec2(-curr_dir.y, curr_dir.x);
				curr_dir.x *= 0.5f;
				curr_normal_dir.x *= 0.5f;

				glm::vec2 shader_direction = glm::normalize(glm::vec2(e.ratio.x, dpoly_fn(x) * e.ratio.y));

				auto p0 = glm::vec2(x, poly_fn(x)) * e.ratio + e.basis;
				p0 /= glm::vec2(size_x, size_y); // Rescale the coordinate to 0-1
				p0 -= (1.5f - 2.f * glyph_positions.y) * curr_normal_dir * real_text_size;
				p0 += (1.0f + 2.f * glyph_positions.x) * curr_dir * real_text_size;

				auto type = float(uint8_t(e.text[i]) >> 6);
				float step = 1.f / 8.f;
				float tx = float(e.text[i] & 7) * step;
				float ty = float((e.text[i] & 63) >> 3) * step;

				text_line_vertices.emplace_back(p0, glm::vec2(-1, 1), shader_direction, glm::vec2(tx, ty), type, real_text_size);
				text_line_vertices.emplace_back(p0, glm::vec2(-1, -1), shader_direction, glm::vec2(tx, ty + step), type, real_text_size);
				text_line_vertices.emplace_back(p0, glm::vec2(1, -1), shader_direction, glm::vec2(tx + step, ty + step), type, real_text_size);

				text_line_vertices.emplace_back(p0, glm::vec2(1, -1), shader_direction, glm::vec2(tx + step, ty + step), type, real_text_size);
				text_line_vertices.emplace_back(p0, glm::vec2(1, 1), shader_direction, glm::vec2(tx + step, ty), type, real_text_size);
				text_line_vertices.emplace_back(p0, glm::vec2(-1, 1), shader_direction, glm::vec2(tx, ty), type, real_text_size);

				// First vertex of the line segment
				//text_line_vertices.emplace_back(p0, +curr_normal_dir, +curr_dir, glm::vec2(tx, ty), type, size);
				//text_line_vertices.emplace_back(p0, -curr_normal_dir, +curr_dir, glm::vec2(tx, ty + step), type, size);
				//text_line_vertices.emplace_back(p0, -curr_normal_dir, -curr_dir, glm::vec2(tx + step, ty + step), type, size);
				// Second vertex of the line segment
				//text_line_vertices.emplace_back(p0, -curr_normal_dir, -curr_dir, glm::vec2(tx + step, ty + step), type, size);
				//text_line_vertices.emplace_back(p0, +curr_normal_dir, -curr_dir, glm::vec2(tx + step, ty), type, size);
				//text_line_vertices.emplace_back(p0, +curr_normal_dir, +curr_dir, glm::vec2(tx, ty), type, size);
			}

			float glyph_advance = ((f.glyph_advances[uint8_t(e.text[i])] / 64.f) + ((i != int32_t(e.text.length() - 1)) ? f.kerning(e.text[i], e.text[i + 1]) / 64.f : 0)) * size;
			for(float glyph_length = 0.f; ; x += x_step) {
				auto added_distance = 2.0f * glm::length(glm::vec2(x_step * e.ratio.x, (poly_fn(x) - poly_fn(x + x_step)) * e.ratio.y));
				if(glyph_length + added_distance >= glyph_advance) {
					x += x_step * (glyph_advance - glyph_length) / added_distance;
					break;
				}
				glyph_length += added_distance;
			}
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, text_line_vbo);
	if(text_line_vertices.size() > 0) {
		glBufferData(GL_ARRAY_BUFFER, sizeof(text_line_vertex) * text_line_vertices.size(), &text_line_vertices[0], GL_STATIC_DRAW);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLuint load_dds_texture(simple_fs::directory const& dir, native_string_view file_name) {
	auto file = simple_fs::open_file(dir, file_name);
	if(!bool(file)) {
		auto full_message = std::string("Can't load DDS file ") + simple_fs::native_to_utf8(file_name) + "\n";
#ifdef _WIN64
		OutputDebugStringA(full_message.c_str());
#else
		std::fprintf(stderr, "%s", full_message.c_str());
#endif
		return 0;
	}
	auto content = simple_fs::view_contents(*file);
	uint32_t size_x, size_y;
	uint8_t const* data = (uint8_t const*)(content.data);
	return ogl::SOIL_direct_load_DDS_from_memory(data, content.file_size, size_x, size_y, ogl::SOIL_FLAG_TEXTURE_REPEATS);
}

void load_static_meshes(sys::state& state) {
	struct static_mesh_vertex {
		glm::vec3 position_;
		glm::vec2 normal_;
		glm::vec2 texture_coord_;
	};
	std::vector<static_mesh_vertex> static_mesh_vertices;
	auto old_size = static_mesh_vertices.size();

	{
		static_mesh_vertex smv;
		smv.position_ = glm::vec3(0.f, 0.f, 1.f);
		static_mesh_vertices.push_back(smv);
	}{
		static_mesh_vertex smv;
		smv.position_ = glm::vec3(0.5f, 1.f, 1.f);
		static_mesh_vertices.push_back(smv);
	}{
		static_mesh_vertex smv;
		smv.position_ = glm::vec3(1.f, 0.f, 1.f);
		static_mesh_vertices.push_back(smv);
	}
	state.map_state.map_data.static_mesh_starts.push_back(0);
	state.map_state.map_data.static_mesh_counts.push_back(3);

	auto root = simple_fs::get_root(state.common_fs);
	auto gfx_anims = simple_fs::open_directory(root, NATIVE("gfx/anims"));
	auto f = simple_fs::open_file(gfx_anims, NATIVE("capital_bigben.xac"));
	if(f) {
		parsers::error_handler err(simple_fs::native_to_utf8(simple_fs::get_full_name(*f)));
		auto contents = simple_fs::view_contents(*f);
		emfx::xac_context context{};
		emfx::parse_xac(context, contents.data, contents.data + contents.file_size, err);
		//emfx::finish(context);

		for(auto const& node : context.nodes) {
			for(auto const& mesh : node.meshes) {
				uint32_t vertex_offset = 0;
				for(auto const& sub : mesh.submeshes) {
					for(size_t i = 0; i < sub.indices.size(); i += 3) {
						static_mesh_vertex smv;
						auto index = sub.indices[i + 0] + vertex_offset;
						{
							auto vv = mesh.vertices[index];
							auto vn = mesh.normals[index];
							auto vt = mesh.texcoords[index];
							smv.position_ = glm::vec3(vv.x, vv.y, vv.z);
							smv.normal_ = glm::vec3(vn.x, vn.y, vn.z);
							smv.texture_coord_ = glm::vec2(vt.x, vt.y);
							static_mesh_vertices.push_back(smv);
						}
						index = sub.indices[i + 1] + vertex_offset;
						{
							auto vv = mesh.vertices[index];
							auto vn = mesh.normals[index];
							auto vt = mesh.texcoords[index];
							smv.position_ = glm::vec3(vv.x, vv.y, vv.z);
							smv.normal_ = glm::vec3(vn.x, vn.y, vn.z);
							smv.texture_coord_ = glm::vec2(vt.x, vt.y);
							static_mesh_vertices.push_back(smv);
						}
						index = sub.indices[i + 2] + vertex_offset;
						{
							auto vv = mesh.vertices[index];
							auto vn = mesh.normals[index];
							auto vt = mesh.texcoords[index];
							smv.position_ = glm::vec3(vv.x, vv.y, vv.z);
							smv.normal_ = glm::vec3(vn.x, vn.y, vn.z);
							smv.texture_coord_ = glm::vec2(vt.x, vt.y);
							static_mesh_vertices.push_back(smv);
						}
					}
					vertex_offset += sub.num_vertices;
				}
			}
		}

		state.map_state.map_data.static_mesh_starts.push_back(GLint(old_size));
		state.map_state.map_data.static_mesh_counts.push_back(GLsizei(static_mesh_vertices.size() - old_size));
	} else {
		std::abort();
	}

	glGenBuffers(1, &state.map_state.map_data.static_mesh_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, state.map_state.map_data.static_mesh_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(static_mesh_vertex) * static_mesh_vertices.size(), &static_mesh_vertices[0], GL_STATIC_DRAW);
	glGenVertexArrays(1, &state.map_state.map_data.static_mesh_vao);
	glBindVertexArray(state.map_state.map_data.static_mesh_vao);
	glBindVertexBuffer(0, state.map_state.map_data.static_mesh_vbo, 0, sizeof(static_mesh_vertex)); // Bind the VBO to 0 of the VAO
	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, offsetof(static_mesh_vertex, position_)); // Set up vertex attribute format for the position
	glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, offsetof(static_mesh_vertex, normal_)); // Set up vertex attribute format for the normal direction
	glVertexAttribFormat(2, 2, GL_FLOAT, GL_FALSE, offsetof(static_mesh_vertex, texture_coord_)); // Set up vertex attribute format for the texture coordinates
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribBinding(0, 0);
	glVertexAttribBinding(1, 0);
	glVertexAttribBinding(2, 0);
	glBindVertexArray(0);
}

void display_data::load_map(sys::state& state) {
	auto root = simple_fs::get_root(state.common_fs);
	auto assets_dir = simple_fs::open_directory(root, NATIVE("assets"));
	auto map_dir = simple_fs::open_directory(root, NATIVE("map"));
	auto map_terrain_dir = simple_fs::open_directory(map_dir, NATIVE("terrain"));
	auto map_items = simple_fs::open_directory(root, NATIVE("gfx/mapitems"));

	load_shaders(root);
	load_static_meshes(state);

	terrain_texture_handle = make_gl_texture(&terrain_id_map[0], size_x, size_y, 1);
	set_gltex_parameters(terrain_texture_handle, GL_TEXTURE_2D, GL_NEAREST, GL_CLAMP_TO_EDGE);
	create_meshes();

	provinces_texture_handle = load_province_map(province_id_map, size_x, size_y);

	auto texturesheet = open_file(map_terrain_dir, NATIVE("texturesheet.tga"));
	terrainsheet_texture_handle = load_texture_array_from_file(*texturesheet, 8, 8);

	water_normal = load_dds_texture(map_terrain_dir, NATIVE("sea_normal.dds"));
	colormap_water = load_dds_texture(map_terrain_dir, NATIVE("colormap_water.dds"));
	colormap_terrain = load_dds_texture(map_terrain_dir, NATIVE("colormap.dds"));
	colormap_political = load_dds_texture(map_terrain_dir, NATIVE("colormap_political.dds"));
	overlay = load_dds_texture(map_terrain_dir, NATIVE("map_overlay_tile.dds"));
	stripes_texture = load_dds_texture(map_terrain_dir, NATIVE("stripes.dds"));
	
	river_body_texture = load_dds_texture(assets_dir, NATIVE("river.dds"));
	set_gltex_parameters(river_body_texture, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);

	coastal_border_texture = load_dds_texture(assets_dir, NATIVE("coastborder.dds"));
	set_gltex_parameters(coastal_border_texture, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);
	
	unit_arrow_texture = make_gl_texture(map_items, NATIVE("movearrow.tga"));
	set_gltex_parameters(unit_arrow_texture, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Get the province_color handle
	// province_color is an array of 2 textures, one for province and the other for stripes
	glGenTextures(1, &province_color);
	glBindTexture(GL_TEXTURE_2D_ARRAY, province_color);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 256, 256, 2);
	set_gltex_parameters(province_color, GL_TEXTURE_2D_ARRAY, GL_NEAREST, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Get the province_highlight handle
	glGenTextures(1, &province_highlight);
	glBindTexture(GL_TEXTURE_2D, province_highlight);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 256, 256);
	set_gltex_parameters(province_highlight, GL_TEXTURE_2D, GL_NEAREST, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Get the province_fow handle
	glGenTextures(1, &province_fow);
	glBindTexture(GL_TEXTURE_2D, province_fow);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 256, 256);
	set_gltex_parameters(province_fow, GL_TEXTURE_2D, GL_NEAREST, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	uint32_t province_size = state.world.province_size() + 1;
	province_size += 256 - province_size % 256;

	std::vector<uint32_t> testHighlight(province_size);
	std::vector<uint32_t> testColor(province_size * 4);
	gen_prov_color_texture(province_highlight, testHighlight);

	for(uint32_t i = 0; i < testHighlight.size(); ++i) {
		testHighlight[i] = 255;
	}

	for(uint32_t i = 0; i < testColor.size(); ++i) {
		testColor[i] = 255;
	}

	set_province_color(testColor);
}

} // namespace map

//static_assert(sizeof(std::pair<dcon::text_key, dcon::text_sequence_id>) == sizeof(dcon::text_key) + sizeof(dcon::text_sequence_id));
//static_assert(sizeof(std::pair<text::text_sequence, dcon::text_sequence_id> == sizeof(dcon::text_sequence) + sizeof(dcon::text_sequence_id)));
//static_assert(sizeof(std::pair<ui::gfx_object, dcon::gfx_object_id> == sizeof(ui::gfx_object) + sizeof(dcon::gfx_object_id)));
//static_assert(sizeof(std::pair<ui::gfx_object, dcon::gfx_object_id> == sizeof(ui::gfx_object) + sizeof(dcon::gfx_object_id)));
