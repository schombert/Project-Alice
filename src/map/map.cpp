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
#include "prng.hpp"
#include "demographics.hpp"

#include "xac.hpp"
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
	auto& vertex = map_data.border_vertices[border.start_index + border.count / 4];
	glm::vec2 map_size = glm::vec2(map_data.size_x, map_data.size_y);

	return vertex.position * map_size;
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

namespace map {

void display_data::update_borders(sys::state& state) {

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
		gen_prov_color_texture(textures[texture_province_fow], province_fows);
	} else {
		state.map_state.visible_provinces.clear();
		state.map_state.visible_provinces.resize(state.world.province_size() + 1, true);
		gen_prov_color_texture(textures[texture_province_fow], province_fows);
	}
}

void create_textured_line_vbo(GLuint vbo, std::vector<textured_line_vertex>& data) {
	// Create and populate the border VBO
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

void create_textured_line_vbo(GLuint vbo, std::vector<textured_line_with_width_vertex>& data) {
	// Create and populate the border VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	if(!data.empty())
		glBufferData(GL_ARRAY_BUFFER, sizeof(textured_line_with_width_vertex) * data.size(), data.data(), GL_STATIC_DRAW);
	// Bind the VBO to 0 of the VAO
	glBindVertexBuffer(0, vbo, 0, sizeof(textured_line_with_width_vertex));
	// Set up vertex attribute format for the position
	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, offsetof(textured_line_with_width_vertex, position_));
	// Set up vertex attribute format for the normal direction
	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, offsetof(textured_line_with_width_vertex, normal_direction_));
	// Set up vertex attribute format for the direction
	glVertexAttribFormat(2, 1, GL_FLOAT, GL_FALSE, offsetof(textured_line_with_width_vertex, texture_coordinate_));
	// Set up vertex attribute format for the texture coordinates
	glVertexAttribFormat(3, 1, GL_FLOAT, GL_FALSE, offsetof(textured_line_with_width_vertex, distance_));
	// Set up vertex attribute format for the width
	glVertexAttribFormat(4, 1, GL_FLOAT, GL_FALSE, offsetof(textured_line_with_width_vertex, width_));
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

void create_textured_line_b_vbo(GLuint vbo, std::vector<textured_line_vertex_b>& data) {
	// Create and populate the border VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	if(!data.empty())
		glBufferData(GL_ARRAY_BUFFER, sizeof(textured_line_vertex_b) * data.size(), data.data(), GL_DYNAMIC_DRAW);
	// Bind the VBO to 0 of the VAO
	glBindVertexBuffer(0, vbo, 0, sizeof(textured_line_vertex_b));
	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, offsetof(textured_line_vertex_b, position));
	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, offsetof(textured_line_vertex_b, previous_point));
	glVertexAttribFormat(2, 2, GL_FLOAT, GL_FALSE, offsetof(textured_line_vertex_b, next_point));
	glVertexAttribFormat(4, 1, GL_FLOAT, GL_FALSE, offsetof(textured_line_vertex_b, texture_coordinate));
	glVertexAttribFormat(5, 1, GL_FLOAT, GL_FALSE, offsetof(textured_line_vertex_b, distance));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	//glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glVertexAttribBinding(0, 0);
	glVertexAttribBinding(1, 0);
	glVertexAttribBinding(2, 0);
	//glVertexAttribBinding(3, 0);
	glVertexAttribBinding(4, 0);
	glVertexAttribBinding(5, 0);
}

void create_textured_line_b_vbo_enriched_with_province_index(GLuint vbo, std::vector<textured_line_vertex_b_enriched_with_province_index>& data) {
	// Create and populate the border VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	if(!data.empty())
		glBufferData(GL_ARRAY_BUFFER, sizeof(textured_line_vertex_b_enriched_with_province_index) * data.size(), data.data(), GL_STATIC_DRAW);
	// Bind the VBO to 0 of the VAO
	glBindVertexBuffer(0, vbo, 0, sizeof(textured_line_vertex_b_enriched_with_province_index));
	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, offsetof(textured_line_vertex_b_enriched_with_province_index, position));
	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, offsetof(textured_line_vertex_b_enriched_with_province_index, previous_point));
	glVertexAttribFormat(2, 2, GL_FLOAT, GL_FALSE, offsetof(textured_line_vertex_b_enriched_with_province_index, next_point));
	glVertexAttribFormat(3, 2, GL_UNSIGNED_BYTE, GL_TRUE, offsetof(textured_line_vertex_b_enriched_with_province_index, province_index));
	glVertexAttribFormat(4, 1, GL_FLOAT, GL_FALSE, offsetof(textured_line_vertex_b_enriched_with_province_index, texture_coordinate));
	glVertexAttribFormat(5, 1, GL_FLOAT, GL_FALSE, offsetof(textured_line_vertex_b_enriched_with_province_index, distance));
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

void create_unit_arrow_vbo(GLuint vbo, std::vector<curved_line_vertex>& data) {
	// Create and populate the border VBO
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

void create_text_line_vbo(GLuint vbo) {
	// Create and populate the border VBO
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
	glVertexAttribFormat(3, 3, GL_FLOAT, GL_FALSE, offsetof(text_line_vertex, texture_coord_));
	glVertexAttribFormat(4, 1, GL_FLOAT, GL_FALSE, offsetof(text_line_vertex, thickness_));
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

void create_drag_box_vbo(GLuint vbo) {
	// Create and populate the border VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Bind the VBO to 0 of the VAO
	glBindVertexBuffer(0, vbo, 0, sizeof(screen_vertex));
	// Set up vertex attribute format for the position
	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, offsetof(screen_vertex, position_));
	glEnableVertexAttribArray(0);
	glVertexAttribBinding(0, 0);
}

void create_square_vbo(GLuint vbo) {
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindVertexBuffer(0, vbo, 0, sizeof(map_vertex));
	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, offsetof(map_vertex, position_));
	glEnableVertexAttribArray(0);
	glVertexAttribBinding(0, 0);

	map_vertex data[4] = {
		{ -1.f, -1.f },
		{  1.f, -1.f },
		{ -1.f,  1.f },
		{  1.f, 1.f  }
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(map_vertex) * 4, data, GL_STATIC_DRAW);
}

void display_data::create_border_ogl_objects() {
	// TODO: remove unused function
}

void display_data::update_borders_mesh() {
	if(border_vertices.empty()) return;
	glBindBuffer(GL_ARRAY_BUFFER, vbo_array[vo_border]);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(textured_line_vertex_b_enriched_with_province_index)
		* border_vertices.size(),
		border_vertices.data(),
		GL_DYNAMIC_DRAW
	);
}

void display_data::create_meshes() {
	std::vector<map_vertex> land_vertices;

	auto add_vertex = [map_size = glm::vec2(float(size_x), float(size_y))](std::vector<map_vertex>& vertices, glm::vec2 pos0) {
		vertices.emplace_back(pos0.x, pos0.y);
	};

	glm::vec2 last_pos(0, 0);
	glm::vec2 pos(0, 0);
	glm::vec2 map_size(size_x, size_y);
	glm::ivec2 sections(200, 200);
	for(int y = 0; y <= sections.y; y++) {
		pos.y = float(y) / float(sections.y);
		for(int x = 0; x <= sections.x; x++) {
			pos.x = float(x) / float(sections.x);
			add_vertex(land_vertices, pos);
		}
	}
	
	map_indices.clear();
	for(int y = 0; y < sections.y; y++) {
		auto top_row_start = y * (sections.x + 1);
		auto bottom_row_start = (y + 1) * (sections.x + 1);
		map_indices.push_back(uint16_t(bottom_row_start + 0));
		map_indices.push_back(uint16_t(top_row_start + 0));
		for(int x = 0; x < sections.x; x++) {
			map_indices.push_back(uint16_t(bottom_row_start + 1 + x));
			map_indices.push_back(uint16_t(top_row_start + 1 + x));
		}
		// primitive restart marker
		map_indices.push_back(std::numeric_limits<uint16_t>::max());
	}

	land_vertex_count = ((uint32_t)land_vertices.size());

	// Fill and bind the VAO
	glBindVertexArray(vao_array[vo_land]);
	// Create and populate the VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo_array[vo_land]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(map_vertex) * land_vertices.size(), land_vertices.data(), GL_STATIC_DRAW);
	// Bind the VBO to 0 of the VAO
	glBindVertexBuffer(0, vbo_array[vo_land], 0, sizeof(map_vertex));
	// Set up vertex attribute format for the position
	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, offsetof(map_vertex, position_));
	glEnableVertexAttribArray(0);
	glVertexAttribBinding(0, 0);

	// Fill and bind the VAOs and VBOs
	glBindVertexArray(vao_array[vo_border]);
	create_textured_line_b_vbo_enriched_with_province_index(vbo_array[vo_border], border_vertices);
	glBindVertexArray(vao_array[vo_river]);
	create_textured_line_vbo(vbo_array[vo_river], river_vertices);
	glBindVertexArray(vao_array[vo_railroad]);
	create_textured_line_vbo(vbo_array[vo_railroad], railroad_vertices);
	glBindVertexArray(vao_array[vo_coastal]);
	create_textured_line_b_vbo(vbo_array[vo_coastal], coastal_vertices);
	glBindVertexArray(vao_array[vo_trade_flow]);
	create_textured_line_vbo(vbo_array[vo_trade_flow], trade_flow_vertices);
	glBindVertexArray(vao_array[vo_unit_arrow]);
	create_unit_arrow_vbo(vbo_array[vo_unit_arrow], unit_arrow_vertices);
	glBindVertexArray(vao_array[vo_attack_unit_arrow]);
	create_unit_arrow_vbo(vbo_array[vo_attack_unit_arrow], attack_unit_arrow_vertices);
	glBindVertexArray(vao_array[vo_retreat_unit_arrow]);
	create_unit_arrow_vbo(vbo_array[vo_retreat_unit_arrow], retreat_unit_arrow_vertices);
	glBindVertexArray(vao_array[vo_strategy_unit_arrow]);
	create_unit_arrow_vbo(vbo_array[vo_strategy_unit_arrow], strategy_unit_arrow_vertices);
	glBindVertexArray(vao_array[vo_objective_unit_arrow]);
	create_unit_arrow_vbo(vbo_array[vo_objective_unit_arrow], objective_unit_arrow_vertices);
	glBindVertexArray(vao_array[vo_other_objective_unit_arrow]);
	create_unit_arrow_vbo(vbo_array[vo_other_objective_unit_arrow], other_objective_unit_arrow_vertices);
	glBindVertexArray(vao_array[vo_text_line]);
	create_text_line_vbo(vbo_array[vo_text_line]);
	glBindVertexArray(vao_array[vo_province_text_line]);
	create_text_line_vbo(vbo_array[vo_province_text_line]);
	glBindVertexArray(vao_array[vo_drag_box]);
	create_drag_box_vbo(vbo_array[vo_drag_box]);
	glBindVertexArray(vao_array[vo_square]);
	create_square_vbo(vbo_array[vo_square]);
	glBindVertexArray(0);
}

display_data::~display_data() {
	/* We don't need to check against 0, since the delete functions already do that for us */
	if(textures[0])
		glDeleteTextures(texture_count, textures);
	if(texture_arrays[0])
		glDeleteTextures(texture_count, texture_arrays);
#if 0
	if(static_mesh_textures[0])
		glDeleteTextures(max_static_meshes, static_mesh_textures);
#endif
	if(vao_array[0])
		glDeleteVertexArrays(vo_count, vao_array);
	if(vbo_array[0])
		glDeleteBuffers(vo_count, vbo_array);

	/* Flags shader for deletion, but doesn't delete them until they're no longer in the rendering context */
	for(const auto shader : shaders) {
		if(shader)
			glDeleteProgram(shader);
	}
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
	auto map_vshader = try_load_shader(root, NATIVE("assets/shaders/glsl/map_v.glsl"));
	auto map_fshader = try_load_shader(root, NATIVE("assets/shaders/glsl/map_f.glsl"));
	auto map_provinces_shader = try_load_shader(root, NATIVE("assets/shaders/glsl/map_provinces_f.glsl"));
	auto screen_vshader = try_load_shader(root, NATIVE("assets/shaders/glsl/screen_v.glsl"));
	auto white_color_fshader = try_load_shader(root, NATIVE("assets/shaders/glsl/white_color_f.glsl"));

	// On-map sprite shader
	auto sprite_vshader = try_load_shader(root, NATIVE("assets/shaders/glsl/map_sprite_v.glsl"));
	auto sprite_fshader = try_load_shader(root, NATIVE("assets/shaders/glsl/map_sprite_f.glsl"));

	// Line shaders
	auto line_unit_arrow_vshader = try_load_shader(root, NATIVE("assets/shaders/glsl/line_unit_arrow_v.glsl"));
	auto line_unit_arrow_fshader = try_load_shader(root, NATIVE("assets/shaders/glsl/line_unit_arrow_f.glsl"));

	auto text_line_vshader = try_load_shader(root, NATIVE("assets/shaders/glsl/text_line_v.glsl"));
	auto text_line_fshader = try_load_shader(root, NATIVE("assets/shaders/glsl/text_line_f.glsl"));

	auto tline_vshader = try_load_shader(root, NATIVE("assets/shaders/glsl/textured_line_v.glsl"));
	auto tline_width_vshader = try_load_shader(root, NATIVE("assets/shaders/glsl/textured_line_variable_width_v.glsl"));
	auto tline_fshader = try_load_shader(root, NATIVE("assets/shaders/glsl/textured_line_f.glsl"));
	auto river_fshader = try_load_shader(root, NATIVE("assets/shaders/glsl/textured_line_river_f.glsl"));

	auto tlineb_vshader = try_load_shader(root, NATIVE("assets/shaders/glsl/textured_line_b_v.glsl"));
	auto tlineb_fshader = try_load_shader(root, NATIVE("assets/shaders/glsl/textured_line_b_f.glsl"));
	auto tlineb_provinces_fshader = try_load_shader(root, NATIVE("assets/shaders/glsl/textured_line_b_provinces_f.glsl"));

	auto model3d_vshader = try_load_shader(root, NATIVE("assets/shaders/glsl/model3d_v.glsl"));
	auto model3d_fshader = try_load_shader(root, NATIVE("assets/shaders/glsl/model3d_f.glsl"));

	shaders[shader_terrain] = create_program(*map_vshader, *map_fshader);
	shaders[shader_provinces] = create_program(*map_vshader, *map_provinces_shader);
	shaders[shader_textured_line] = create_program(*tline_vshader, *tline_fshader);
	shaders[shader_textured_line_with_variable_width] = create_program(*tline_width_vshader, *river_fshader);
	shaders[shader_trade_flow] = create_program(*tline_width_vshader, *tlineb_fshader);
	shaders[shader_railroad_line] = create_program(*tline_vshader, *tlineb_fshader);
	shaders[shader_borders] = create_program(*tlineb_vshader, *tlineb_fshader);
	shaders[shader_borders_provinces] = create_program(*tlineb_vshader, *tlineb_provinces_fshader);
	shaders[shader_line_unit_arrow] = create_program(*line_unit_arrow_vshader, *line_unit_arrow_fshader);
	shaders[shader_text_line] = create_program(*text_line_vshader, *text_line_fshader);
	shaders[shader_drag_box] = create_program(*screen_vshader, *white_color_fshader);
	shaders[shader_map_standing_object] = create_program(*model3d_vshader, *model3d_fshader);
	shaders[shader_map_sprite] = create_program(*sprite_vshader, *sprite_fshader);

	for(uint32_t i = 0; i < shader_count; i++) {
		if(shaders[i] == 0)
			continue;
		shader_uniforms[i][uniform_provinces_texture_sampler] = glGetUniformLocation(shaders[i], "provinces_texture_sampler");
		shader_uniforms[i][uniform_provinces_real_texture_sampler] = glGetUniformLocation(shaders[i], "real_provinces_texture_sampler");
		shader_uniforms[i][uniform_provinces_sea_mask] = glGetUniformLocation(shaders[i], "provinces_sea_mask");
		shader_uniforms[i][uniform_offset] = glGetUniformLocation(shaders[i], "offset");
		shader_uniforms[i][uniform_aspect_ratio] = glGetUniformLocation(shaders[i], "aspect_ratio");
		shader_uniforms[i][uniform_zoom] = glGetUniformLocation(shaders[i], "zoom");
		shader_uniforms[i][uniform_map_size] = glGetUniformLocation(shaders[i], "map_size");
		shader_uniforms[i][uniform_screen_size] = glGetUniformLocation(shaders[i], "screen_size");
		shader_uniforms[i][uniform_rotation] = glGetUniformLocation(shaders[i], "rotation");
		shader_uniforms[i][uniform_gamma] = glGetUniformLocation(shaders[i], "gamma");
		shader_uniforms[i][uniform_subroutines_index] = glGetUniformLocation(shaders[i], "subroutines_index");
		shader_uniforms[i][uniform_time] = glGetUniformLocation(shaders[i], "time");
		shader_uniforms[i][uniform_terrain_texture_sampler] = glGetUniformLocation(shaders[i], "terrain_texture_sampler");
		shader_uniforms[i][uniform_terrainsheet_texture_sampler] = glGetUniformLocation(shaders[i], "terrainsheet_texture_sampler");
		shader_uniforms[i][uniform_water_normal] = glGetUniformLocation(shaders[i], "water_normal");
		shader_uniforms[i][uniform_colormap_water] = glGetUniformLocation(shaders[i], "colormap_water");
		shader_uniforms[i][uniform_colormap_terrain] = glGetUniformLocation(shaders[i], "colormap_terrain");
		shader_uniforms[i][uniform_overlay] = glGetUniformLocation(shaders[i], "overlay");
		shader_uniforms[i][uniform_province_color] = glGetUniformLocation(shaders[i], "province_color");
		shader_uniforms[i][uniform_colormap_political] = glGetUniformLocation(shaders[i], "colormap_political");
		shader_uniforms[i][uniform_province_highlight] = glGetUniformLocation(shaders[i], "province_highlight");
		shader_uniforms[i][uniform_stripes_texture] = glGetUniformLocation(shaders[i], "stripes_texture");
		shader_uniforms[i][uniform_province_fow] = glGetUniformLocation(shaders[i], "province_fow");
		shader_uniforms[i][uniform_diag_border_identifier] = glGetUniformLocation(shaders[i], "diag_border_identifier");
		shader_uniforms[i][uniform_subroutines_index_2] = glGetUniformLocation(shaders[i], "subroutines_index_2");
		shader_uniforms[i][uniform_line_texture] = glGetUniformLocation(shaders[i], "line_texture");
		shader_uniforms[i][uniform_texture_sampler] = glGetUniformLocation(shaders[i], "texture_sampler");
		shader_uniforms[i][uniform_opaque] = glGetUniformLocation(shaders[i], "opaque");
		shader_uniforms[i][uniform_is_black] = glGetUniformLocation(shaders[i], "is_black");
		shader_uniforms[i][uniform_border_width] = glGetUniformLocation(shaders[i], "border_width");
		shader_uniforms[i][uniform_unit_arrow] = glGetUniformLocation(shaders[i], "unit_arrow");
		shader_uniforms[i][uniform_model_offset] = glGetUniformLocation(shaders[i], "model_offset");
		shader_uniforms[i][uniform_target_facing] = glGetUniformLocation(shaders[i], "target_facing");
		shader_uniforms[i][uniform_target_topview_fixup] = glGetUniformLocation(shaders[i], "target_topview_fixup");
		shader_uniforms[i][uniform_width] = glGetUniformLocation(shaders[i], "width");
		shader_uniforms[i][uniform_sprite_offsets] = glGetUniformLocation(shaders[i], "position_offset");
		shader_uniforms[i][uniform_sprite_scale] = glGetUniformLocation(shaders[i], "scale");
		shader_uniforms[i][uniform_sprite_texture_start] = glGetUniformLocation(shaders[i], "texture_start");
		shader_uniforms[i][uniform_sprite_texture_size] = glGetUniformLocation(shaders[i], "texture_size");
		shader_uniforms[i][uniform_is_national_border] = glGetUniformLocation(shaders[i], "is_national_border");
		shader_uniforms[i][uniform_graphics_mode] = glGetUniformLocation(shaders[i], "graphics_mode");
	}
}

void display_data::render(sys::state& state, glm::vec2 screen_size, glm::vec2 offset, float zoom, map_view map_view_mode, map_mode::mode active_map_mode, glm::mat3 globe_rotation, float time_counter) {
	if(!screen_size.x || !screen_size.y) {
		return;
	}

	// Load general shader stuff, used by both land and borders
	auto load_shader = [&](GLuint program) {
		glUseProgram(shaders[program]);
		glUniform2f(shader_uniforms[program][uniform_offset], offset.x + 0.f, offset.y);
		glUniform1f(shader_uniforms[program][uniform_aspect_ratio], screen_size.x / screen_size.y);
		glUniform2f(shader_uniforms[program][uniform_screen_size], screen_size.x, screen_size.y);
		glUniform1f(shader_uniforms[program][uniform_zoom], zoom);
		glUniform2f(shader_uniforms[program][uniform_map_size], GLfloat(size_x), GLfloat(size_y));
		glUniformMatrix3fv(shader_uniforms[program][uniform_rotation], 1, GL_FALSE, glm::value_ptr(glm::mat3(globe_rotation)));
		glUniform1f(shader_uniforms[program][uniform_gamma], state.user_settings.gamma);
		glUniform1ui(shader_uniforms[program][uniform_subroutines_index], GLuint(map_view_mode));
		glUniform1f(shader_uniforms[program][uniform_time], time_counter);
	};	

	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(std::numeric_limits<uint16_t>::max());

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, state.open_gl.province_map_framebuffer);
	glViewport(0, 0, int(screen_size.x), int(screen_size.y));

	glClearColor(0.5f, 0.5f, 0.5f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_2D, textures[texture_provinces]);

	load_shader(shader_provinces);
	glUniform1i(shader_uniforms[shader_provinces][uniform_provinces_real_texture_sampler], 12);
	glBindVertexArray(vao_array[vo_land]);

	glDisable(GL_CULL_FACE);
	glDrawElements(GL_TRIANGLE_STRIP, GLsizei(map_indices.size() - 1), GL_UNSIGNED_SHORT, map_indices.data());
	glDisable(GL_PRIMITIVE_RESTART);

	// BORDERS TO FIX HUGE PIXELS
	if(state.user_settings.graphics_mode != sys::graphics_mode::ugly) {
		load_shader(shader_borders_provinces);
		glUniform1i(shader_uniforms[shader_borders_provinces][uniform_provinces_real_texture_sampler], 12);
		glBindVertexArray(vao_array[vo_border]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_array[vo_border]);

		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);
		glClearDepth(1.f);
		glDepthFunc(GL_LESS);

		for(auto b : borders) {
			glUniform1f(shader_uniforms[shader_borders_provinces][uniform_width], 0.002f); // width
			if(b.count == 0) continue;
			if(
				!b.adj
				||
				(
					state.world.province_adjacency_get_type(b.adj)
					&
					(
						province::border::coastal_bit
						| province::border::national_bit
						| province::border::impassible_bit
					)
				)
			) {
				glUniform1f(shader_uniforms[shader_borders_provinces][uniform_is_national_border], 1.f);
			} else {
				glUniform1f(shader_uniforms[shader_borders_provinces][uniform_is_national_border], 0.f);
			}

			glDrawArrays(GL_TRIANGLE_STRIP, b.start_index, b.count / 2);
			glDrawArrays(GL_TRIANGLE_STRIP, b.start_index + b.count / 2, b.count / 2);
		}
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	
	if(ogl::msaa_enabled(state)) {
		glBindFramebuffer(GL_FRAMEBUFFER, state.open_gl.msaa_framebuffer);
		glClearColor(1.f, 1.f, 1.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, state.open_gl.province_map_rendertexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures[texture_terrain]);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture_arrays[texture_array_terrainsheet]);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, textures[texture_water_normal]);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, textures[texture_colormap_water]);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, textures[texture_colormap_terrain]);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, textures[texture_overlay]);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture_arrays[texture_array_province_color]);
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, textures[texture_colormap_political]);
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, textures[texture_province_highlight]);
	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, textures[texture_stripes]);
	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_2D, textures[texture_sea_mask]);
	glActiveTexture(GL_TEXTURE13);
	glBindTexture(GL_TEXTURE_2D, textures[texture_province_fow]);
	glActiveTexture(GL_TEXTURE14);
	glBindTexture(GL_TEXTURE_2D, textures[texture_river_body]);
	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_2D, textures[texture_diag_border_identifier]);

	load_shader(shader_terrain);
	glUniform1i(shader_uniforms[shader_terrain][uniform_provinces_texture_sampler], 0);
	glUniform1i(shader_uniforms[shader_terrain][uniform_terrain_texture_sampler], 1);
	//glUniform1i(shader_uniforms[shader_terrain][uniform_unused_texture_2], 2);
	glUniform1i(shader_uniforms[shader_terrain][uniform_terrainsheet_texture_sampler], 3);
	glUniform1i(shader_uniforms[shader_terrain][uniform_water_normal], 4);
	glUniform1i(shader_uniforms[shader_terrain][uniform_colormap_water], 5);
	glUniform1i(shader_uniforms[shader_terrain][uniform_colormap_terrain], 6);
	glUniform1i(shader_uniforms[shader_terrain][uniform_overlay], 7);
	glUniform1i(shader_uniforms[shader_terrain][uniform_province_color], 8);
	glUniform1i(shader_uniforms[shader_terrain][uniform_colormap_political], 9);
	glUniform1i(shader_uniforms[shader_terrain][uniform_province_highlight], 10);
	glUniform1i(shader_uniforms[shader_terrain][uniform_stripes_texture], 11);
	glUniform1i(shader_uniforms[shader_terrain][uniform_provinces_sea_mask], 12);
	glUniform1i(shader_uniforms[shader_terrain][uniform_province_fow], 13);
	//glUniform1i(shader_uniforms[shader_terrain][uniform_unused_texture_14], 14);
	glUniform1i(shader_uniforms[shader_terrain][uniform_diag_border_identifier], 15);
	{ // Land specific shader uniform
		// get_land()
		GLuint fragment_subroutines = 0;
		if(active_map_mode == map_mode::mode::terrain)
			fragment_subroutines = 0; // get_land_terrain/get_water_terrain()
		else if(zoom > map::zoom_close)
			fragment_subroutines = 1; // get_land_political_close/get_water_terrain()
		else
			fragment_subroutines = 2; // get_land_political_far/get_water_political()
		glUniform1ui(shader_uniforms[shader_terrain][uniform_subroutines_index_2], fragment_subroutines);

		glUniform1ui(shader_uniforms[shader_terrain][uniform_graphics_mode], (uint8_t)state.user_settings.graphics_mode);
	}

	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(std::numeric_limits<uint16_t>::max());
	glBindVertexArray(vao_array[vo_land]);
	glDrawElements(GL_TRIANGLE_STRIP, GLsizei(map_indices.size() - 1), GL_UNSIGNED_SHORT, map_indices.data());

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDisable(GL_PRIMITIVE_RESTART);

	// Draw the rivers
	if(state.user_settings.rivers_enabled) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[texture_river_body]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[texture_colormap_water]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textures[texture_sea_mask]);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, state.open_gl.province_map_rendertexture);


		load_shader(shader_textured_line_with_variable_width);
		glUniform1i(shader_uniforms[shader_textured_line_with_variable_width][uniform_line_texture], 0);
		glUniform1i(shader_uniforms[shader_textured_line_with_variable_width][uniform_colormap_water], 1);
		glUniform1i(shader_uniforms[shader_textured_line_with_variable_width][uniform_provinces_sea_mask], 2);
		glUniform1i(shader_uniforms[shader_textured_line_with_variable_width][uniform_provinces_texture_sampler], 3);

		glBindVertexArray(vao_array[vo_river]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_array[vo_river]);
		glMultiDrawArrays(GL_TRIANGLE_STRIP, river_starts.data(), river_counts.data(), GLsizei(river_starts.size()));
	}

	

	// Draw the railroads
	if(zoom > map::zoom_close && !railroad_vertices.empty()) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[texture_railroad]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[texture_colormap_water]);

		load_shader(shader_railroad_line);
		glUniform1i(shader_uniforms[shader_railroad_line][uniform_line_texture], 0);
		glUniform1i(shader_uniforms[shader_railroad_line][uniform_colormap_water], 1);
		glUniform1f(shader_uniforms[shader_railroad_line][uniform_width], 0.0001f);
		glUniform1f(shader_uniforms[shader_railroad_line][uniform_time], 0.f);

		glBindVertexArray(vao_array[vo_railroad]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_array[vo_railroad]);
		glMultiDrawArrays(GL_TRIANGLE_STRIP, railroad_starts.data(), railroad_counts.data(), GLsizei(railroad_starts.size()));
	}

	// Default border parameters
	constexpr float border_type_national = 0.f;
	constexpr float border_type_provincial = 1.f;
	constexpr float border_type_regional = 2.f;
	constexpr float border_type_coastal = 3.f;

	// NORMAL BORDERS
	load_shader(shader_borders);
	glUniform1i(shader_uniforms[shader_borders][uniform_provinces_texture_sampler], 0);
	glUniform1i(shader_uniforms[shader_borders][uniform_province_fow], 1);
	glUniform1i(shader_uniforms[shader_borders][uniform_line_texture], 2);

	glBindVertexArray(vao_array[vo_border]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_array[vo_border]);

	if(state.user_settings.graphics_mode != sys::graphics_mode::ugly) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[texture_provinces]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[texture_province_fow]);
		if(zoom > map::zoom_close) {
			if(zoom > map::zoom_very_close) { // Render province borders
				glUniform1f(shader_uniforms[shader_borders][uniform_width], 0.0001f); // width
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, textures[texture_prov_border]);

				for(auto b : borders) {
					if(!b.adj) continue;
					if((state.world.province_adjacency_get_type(b.adj) & (province::border::non_adjacent_bit | province::border::coastal_bit | province::border::impassible_bit | province::border::national_bit | province::border::state_bit)) == 0) {
						glDrawArrays(GL_TRIANGLE_STRIP, b.start_index, b.count / 2);
						glDrawArrays(GL_TRIANGLE_STRIP, b.start_index + b.count / 2, b.count / 2);
					}
				}
			}
			{ // Render state borders
				glUniform1f(shader_uniforms[shader_borders][uniform_width], 0.0001f); // width
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, textures[texture_state_border]);
				for(auto b : borders) {
					if(!b.adj) continue;
					if((state.world.province_adjacency_get_type(b.adj) & (province::border::non_adjacent_bit | province::border::coastal_bit | province::border::impassible_bit | province::border::national_bit | province::border::state_bit)) == province::border::state_bit) {
						glDrawArrays(GL_TRIANGLE_STRIP, b.start_index, b.count / 2);
						glDrawArrays(GL_TRIANGLE_STRIP, b.start_index + b.count / 2, b.count / 2);
					}
				}
			}
			// impassible borders
			{
				glUniform1f(shader_uniforms[shader_borders][uniform_width], 0.0002f); // width
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, textures[texture_imp_border]);
				for(auto b : borders) {
					if(!b.adj) continue;
					if((state.world.province_adjacency_get_type(b.adj) & (province::border::non_adjacent_bit | province::border::coastal_bit | province::border::impassible_bit)) == province::border::impassible_bit) {
						glDrawArrays(GL_TRIANGLE_STRIP, b.start_index, b.count / 2);
						glDrawArrays(GL_TRIANGLE_STRIP, b.start_index + b.count / 2, b.count / 2);
					}
				}
			}
			// national borders
			{
				glUniform1f(shader_uniforms[shader_borders][uniform_width], 0.0001f); // width
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, textures[texture_national_border]);
				for(auto b : borders) {
					if(!b.adj) continue;
					if((state.world.province_adjacency_get_type(b.adj) & (province::border::non_adjacent_bit | province::border::coastal_bit | province::border::national_bit | province::border::impassible_bit)) == province::border::national_bit) {
						glDrawArrays(GL_TRIANGLE_STRIP, b.start_index, b.count / 2);
						glDrawArrays(GL_TRIANGLE_STRIP, b.start_index + b.count / 2, b.count / 2);
					}
				}
			}
		} else {
			if(zoom > map::zoom_very_close) { // Render province borders
				glUniform1f(shader_uniforms[shader_borders][uniform_width], 0.00005f); // width
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, textures[texture_prov_border]);
				for(auto b : borders) {
					if(!b.adj) continue;
					if((state.world.province_adjacency_get_type(b.adj) & (province::border::non_adjacent_bit | province::border::coastal_bit | province::border::national_bit | province::border::state_bit)) == 0) {
						glDrawArrays(GL_TRIANGLE_STRIP, b.start_index, b.count / 2);
						glDrawArrays(GL_TRIANGLE_STRIP, b.start_index + b.count / 2, b.count / 2);
					}
				}
			}
			if(zoom > map::zoom_close) { // Render state borders
				glUniform1f(shader_uniforms[shader_borders][uniform_width], 0.00005f); // width
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, textures[texture_state_border]);
				for(auto b : borders) {
					if(!b.adj) continue;
					if((state.world.province_adjacency_get_type(b.adj) & (province::border::non_adjacent_bit | province::border::coastal_bit | province::border::national_bit | province::border::state_bit)) == province::border::state_bit) {
						glDrawArrays(GL_TRIANGLE_STRIP, b.start_index, b.count / 2);
						glDrawArrays(GL_TRIANGLE_STRIP, b.start_index + b.count / 2, b.count / 2);
					}
				}
			}
			// national borders
			{
				glUniform1f(shader_uniforms[shader_borders][uniform_width], 0.0003f); // width
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, textures[texture_state_border]);
				for(auto b : borders) {
					if(!b.adj) continue;
					if((state.world.province_adjacency_get_type(b.adj) & (province::border::non_adjacent_bit | province::border::coastal_bit | province::border::national_bit)) == province::border::national_bit) {
						glDrawArrays(GL_TRIANGLE_STRIP, b.start_index, b.count / 2);
						glDrawArrays(GL_TRIANGLE_STRIP, b.start_index + b.count / 2, b.count / 2);
					}
				}
			}
		}
	}
	

	if(state.map_state.selected_province || (state.local_player_nation && state.current_scene.borders == game_scene::borders_granularity::nation)) {
		glUniform1f(shader_uniforms[shader_borders][uniform_width], zoom > map::zoom_close ? 0.0004f : 0.00085f); // width
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textures[texture_state_border]);
		if(state.local_player_nation && state.current_scene.borders == game_scene::borders_granularity::nation) {
			for(auto b : borders) {
				if(!b.adj) continue;
				auto p0 = state.world.province_adjacency_get_connected_provinces(b.adj, 0);
				auto p1 = state.world.province_adjacency_get_connected_provinces(b.adj, 1);
				if((state.world.province_get_nation_from_province_ownership(p0) == state.local_player_nation
					|| state.world.province_get_nation_from_province_ownership(p1) == state.local_player_nation)
				&& (state.world.province_adjacency_get_type(b.adj) & (province::border::non_adjacent_bit | province::border::coastal_bit | province::border::national_bit)) != 0) {
					glDrawArrays(GL_TRIANGLE_STRIP, b.start_index, b.count / 2);
					glDrawArrays(GL_TRIANGLE_STRIP, b.start_index + b.count / 2, b.count / 2);
				}
			}
		} else if(state.current_scene.borders == game_scene::borders_granularity::state) {
			auto owner = state.world.province_get_nation_from_province_ownership(state.map_state.selected_province);
			if(owner) {
				auto siid = state.world.province_get_state_membership(state.map_state.selected_province);
				//per state
				for(auto b : borders) {
					if(!b.adj) continue;
					auto p0 = state.world.province_adjacency_get_connected_provinces(b.adj, 0);
					auto p1 = state.world.province_adjacency_get_connected_provinces(b.adj, 1);
					if((state.world.province_get_state_membership(p0) == siid
						|| state.world.province_get_state_membership(p1) == siid)
					&& (state.world.province_adjacency_get_type(b.adj) & (province::border::non_adjacent_bit | province::border::coastal_bit | province::border::state_bit | province::border::national_bit)) != 0) {
						glDrawArrays(GL_TRIANGLE_STRIP, b.start_index, b.count / 2);
						glDrawArrays(GL_TRIANGLE_STRIP, b.start_index + b.count / 2, b.count / 2);
					}
				}
			}
		} else if(state.current_scene.borders == game_scene::borders_granularity::province) {
			for(auto b : borders) {
				if(!b.adj) continue;
				auto p0 = state.world.province_adjacency_get_connected_provinces(b.adj, 0);
				auto p1 = state.world.province_adjacency_get_connected_provinces(b.adj, 1);
				if(p0 == state.map_state.selected_province || p1 == state.map_state.selected_province) {
					glDrawArrays(GL_TRIANGLE_STRIP, b.start_index, b.count / 2);
					glDrawArrays(GL_TRIANGLE_STRIP, b.start_index + b.count / 2, b.count / 2);
				}
			}
		}
	}
	dcon::province_id prov{};
	glm::vec2 map_pos;
	if(!state.ui_state.under_mouse && state.map_state.screen_to_map(glm::vec2(state.mouse_x_position, state.mouse_y_position), screen_size, state.map_state.current_view(state), map_pos)) {
		map_pos *= glm::vec2(float(state.map_state.map_data.size_x), float(state.map_state.map_data.size_y));
		auto idx = int32_t(state.map_state.map_data.size_y - map_pos.y) * int32_t(state.map_state.map_data.size_x) + int32_t(map_pos.x);
		if(0 <= idx && size_t(idx) < state.map_state.map_data.province_id_map.size() && state.map_state.map_data.province_id_map[idx] < province::to_map_id(state.province_definitions.first_sea_province)) {
			auto fat_id = dcon::fatten(state.world, province::from_map_id(state.map_state.map_data.province_id_map[idx]));
			prov = province::from_map_id(state.map_state.map_data.province_id_map[idx]);
			glUniform1f(shader_uniforms[shader_borders][uniform_width], zoom > map::zoom_close ? 0.0004f : 0.00085f); // width
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, textures[texture_hover_border]);
			auto owner = state.world.province_get_nation_from_province_ownership(prov);
			if(owner && state.current_scene.borders == game_scene::borders_granularity::nation) {
				//per nation
				for(auto b : borders) {
					if(!b.adj) continue;
					auto p0 = state.world.province_adjacency_get_connected_provinces(b.adj, 0);
					auto p1 = state.world.province_adjacency_get_connected_provinces(b.adj, 1);
					if((state.world.province_get_nation_from_province_ownership(p0) == owner
						|| state.world.province_get_nation_from_province_ownership(p1) == owner)
					&& (state.world.province_adjacency_get_type(b.adj) & (province::border::non_adjacent_bit | province::border::coastal_bit | province::border::national_bit)) != 0) {
						glDrawArrays(GL_TRIANGLE_STRIP, b.start_index, b.count / 2);
						glDrawArrays(GL_TRIANGLE_STRIP, b.start_index + b.count / 2, b.count / 2);
					}
				}
			} else if(owner && state.current_scene.borders == game_scene::borders_granularity::state) {
				auto siid = state.world.province_get_state_membership(prov);
				//per state
				for(auto b : borders) {
					if(!b.adj) continue;
					auto p0 = state.world.province_adjacency_get_connected_provinces(b.adj, 0);
					auto p1 = state.world.province_adjacency_get_connected_provinces(b.adj, 1);
					if((state.world.province_get_state_membership(p0) == siid
						|| state.world.province_get_state_membership(p1) == siid)
					&& (state.world.province_adjacency_get_type(b.adj) & (province::border::non_adjacent_bit | province::border::coastal_bit | province::border::state_bit | province::border::national_bit)) != 0) {
						glDrawArrays(GL_TRIANGLE_STRIP, b.start_index, b.count / 2);
						glDrawArrays(GL_TRIANGLE_STRIP, b.start_index + b.count / 2, b.count / 2);
					}
				}
			} else if(owner && state.current_scene.borders == game_scene::borders_granularity::province)  {
				//per province
				for(auto b : borders) {
					if(!b.adj) continue;
					auto p0 = state.world.province_adjacency_get_connected_provinces(b.adj, 0);
					auto p1 = state.world.province_adjacency_get_connected_provinces(b.adj, 1);
					if(p0 == prov || p1 == prov) {
						glDrawArrays(GL_TRIANGLE_STRIP, b.start_index, b.count / 2);
						glDrawArrays(GL_TRIANGLE_STRIP, b.start_index + b.count / 2, b.count / 2);
					}
				}
			}
		}
	}
	// coasts

	if (state.user_settings.graphics_mode != sys::graphics_mode::ugly) {
		glUniform1f(shader_uniforms[shader_borders][uniform_width], 0.0002f); // width
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textures[texture_coastal_border]);
		glBindVertexArray(vao_array[vo_border]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_array[vo_border]);
		{
			for(auto b : borders) {
				if(!b.adj) {
					glDrawArrays(GL_TRIANGLE_STRIP, b.start_index, b.count / 2);
					glDrawArrays(GL_TRIANGLE_STRIP, b.start_index + b.count / 2, b.count / 2);
				} else if((state.world.province_adjacency_get_type(b.adj) & (province::border::coastal_bit)) == province::border::coastal_bit) {
					glDrawArrays(GL_TRIANGLE_STRIP, b.start_index, b.count / 2);
					glDrawArrays(GL_TRIANGLE_STRIP, b.start_index + b.count / 2, b.count / 2);
				}
			}
		}
	}

	// trade flow
	if(state.selected_trade_good && !trade_flow_vertices.empty()) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[texture_arrow]);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, textures[texture_provinces]);

		load_shader(shader_trade_flow);
		glUniform1i(shader_uniforms[shader_trade_flow][uniform_line_texture], 0);
		glUniform1i(shader_uniforms[shader_trade_flow][uniform_provinces_texture_sampler], 3);

		glBindVertexArray(vao_array[vo_trade_flow]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_array[vo_trade_flow]);
		glMultiDrawArrays(GL_TRIANGLE_STRIP, trade_flow_arrow_starts.data(), trade_flow_arrow_counts.data(), GLsizei(trade_flow_arrow_starts.size()));

		// trade particles
		if(state.user_settings.graphics_mode != sys::graphics_mode::ugly) {
			load_shader(shader_map_sprite);

			auto gfx_id =
				state.ui_defs.gui[
					state.ui_state.defs_by_name.find(
						state.lookup_key("gfx_storage_commodity")
					)->second.definition
				].data.image.gfx_object;
			auto& gfx_def = state.ui_defs.gfx[gfx_id];
			auto frame = state.world.commodity_get_icon(state.selected_trade_good);
			auto texture_handle = ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent());

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture_handle);

			glUniform1i(shader_uniforms[shader_map_sprite][uniform_texture_sampler], 0);
			glUniform2f(shader_uniforms[shader_map_sprite][uniform_sprite_texture_start], (float)frame / gfx_def.number_of_frames, 0.f);
			glUniform2f(shader_uniforms[shader_map_sprite][uniform_sprite_texture_size], 1.f / gfx_def.number_of_frames, 1.f);

			glBindVertexArray(vao_array[vo_square]);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_array[vo_square]);

			const float speed = 0.5f;

			bool spawned_something = false;

			for(size_t i = 0; i < trade_particles_positions.size(); i++) {
				auto& p = trade_particles_positions[i];

				// update movement
				if(p.trade_graph_node_current != -1 && p.trade_graph_node_next != -1) {
					auto direction = p.target_ - p.position_;
					auto length = float(glm::length(direction));
					if(length < speed * 2) {
						p.trade_graph_node_prev = p.trade_graph_node_current;
						p.trade_graph_node_current = p.trade_graph_node_next;
						p.trade_graph_node_next = -1;
					} else {
						p.position_ += direction / length * speed;
					}
				}

				// choose target
				if(p.trade_graph_node_current != -1 && p.trade_graph_node_next == -1) {
					// choose next target according to probability
					// use time as random engine for simplicity
					auto random = fmod(sin(time_counter * 971641.5397643) + 1.f, 1.f);

					int target = -1;

					auto accumulated = 0.f;
					for(auto const& [candidate, probability] : particle_next_node_probability[p.trade_graph_node_current]) {
						accumulated += probability;
						// prevent trivial loops
						if(random < accumulated && candidate != p.trade_graph_node_prev) {
							target = candidate;
							break;
						}
					}

					// moving to itself or having no paths to go out implies deletion
					if(target == -1 || target == p.trade_graph_node_current) {
						p.trade_graph_node_current = -1;
					} else {
						p.trade_graph_node_next = target;
						p.target_ = put_in_local(trade_node_position[target], p.position_, (float)size_x);
					}
				}


				if(p.trade_graph_node_current != -1) {
					glUniform2f(shader_uniforms[shader_map_sprite][uniform_sprite_offsets], trade_particles_positions[i].position_.x / float(size_x), trade_particles_positions[i].position_.y / float(size_y));
					glUniform2f(shader_uniforms[shader_map_sprite][uniform_sprite_scale], 4.f / float(size_x), 4.f / float(size_y));
					glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				}

				// spawn "new" particles
				// don't spawn them too often
				if(!spawned_something && p.trade_graph_node_current == -1) {
					spawned_something = true;

					auto random = fmod(sin(time_counter * 92637.1323076) + 1.f, 1.f);

					int target = -1;
					float accumulated = 0.f;
					for(auto const& [candidate, probability] : particle_creation_probability) {
						accumulated += probability;
						if(random < accumulated) {
							target = candidate;
							break;
						}
					}

					if(target != -1) {
						p.trade_graph_node_current = target;
						p.position_ = trade_node_position[target];
						p.target_ = trade_node_position[target];
						p.trade_graph_node_next = -1;
						p.trade_graph_node_prev = -1;
					}
				}
			}
		}
	}

	if(zoom > map::zoom_close) { //only render if close enough
		if(!unit_arrow_vertices.empty() || !attack_unit_arrow_vertices.empty() || !retreat_unit_arrow_vertices.empty()
		|| !strategy_unit_arrow_vertices.empty() || !objective_unit_arrow_vertices.empty() || !other_objective_unit_arrow_vertices.empty()) {
			load_shader(shader_line_unit_arrow);
			glUniform1f(shader_uniforms[shader_line_unit_arrow][uniform_border_width], 0.005f); //width
			glUniform1i(shader_uniforms[shader_line_unit_arrow][uniform_unit_arrow], 0);
		}
		if(!unit_arrow_vertices.empty()) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[texture_unit_arrow]);
			glBindVertexArray(vao_array[vo_unit_arrow]);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_array[vo_unit_arrow]);
			glMultiDrawArrays(GL_TRIANGLE_STRIP, unit_arrow_starts.data(), unit_arrow_counts.data(), (GLsizei)unit_arrow_counts.size());
		}
		if(!attack_unit_arrow_vertices.empty()) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[texture_attack_unit_arrow]);
			glBindVertexArray(vao_array[vo_attack_unit_arrow]);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_array[vo_attack_unit_arrow]);
			glMultiDrawArrays(GL_TRIANGLE_STRIP, attack_unit_arrow_starts.data(), attack_unit_arrow_counts.data(), (GLsizei)attack_unit_arrow_counts.size());
		}
		if(!retreat_unit_arrow_vertices.empty()) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[texture_retreat_unit_arrow]);
			glBindVertexArray(vao_array[vo_retreat_unit_arrow]);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_array[vo_retreat_unit_arrow]);
			glMultiDrawArrays(GL_TRIANGLE_STRIP, retreat_unit_arrow_starts.data(), retreat_unit_arrow_counts.data(), (GLsizei)retreat_unit_arrow_counts.size());
		}
		if(!strategy_unit_arrow_vertices.empty()) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[texture_strategy_unit_arrow]);
			glBindVertexArray(vao_array[vo_strategy_unit_arrow]);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_array[vo_strategy_unit_arrow]);
			glMultiDrawArrays(GL_TRIANGLE_STRIP, strategy_unit_arrow_starts.data(), strategy_unit_arrow_counts.data(), (GLsizei)strategy_unit_arrow_counts.size());
		}
		if(!objective_unit_arrow_vertices.empty()) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[texture_objective_unit_arrow]);
			glBindVertexArray(vao_array[vo_objective_unit_arrow]);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_array[vo_objective_unit_arrow]);
			glMultiDrawArrays(GL_TRIANGLE_STRIP, objective_unit_arrow_starts.data(), objective_unit_arrow_counts.data(), (GLsizei)objective_unit_arrow_counts.size());
		}
		if(!other_objective_unit_arrow_vertices.empty()) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[texture_other_objective_unit_arrow]);
			glBindVertexArray(vao_array[vo_other_objective_unit_arrow]);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_array[vo_other_objective_unit_arrow]);
			glMultiDrawArrays(GL_TRIANGLE_STRIP, other_objective_unit_arrow_starts.data(), other_objective_unit_arrow_counts.data(), (GLsizei)retreat_unit_arrow_counts.size());
		}
	}

	if(!drag_box_vertices.empty()) {
		glUseProgram(shaders[shader_drag_box]);
		glUniform1f(shader_uniforms[shader_drag_box][uniform_gamma], state.user_settings.gamma);
		glBindVertexArray(vao_array[vo_drag_box]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_array[vo_drag_box]);
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)drag_box_vertices.size());
	}

	if(state.user_settings.map_label != sys::map_label_mode::none) {
		auto const& f = state.font_collection.get_font(state, text::font_selection::map_font);
		load_shader(shader_text_line);
		glUniform1i(shader_uniforms[shader_text_line][uniform_texture_sampler], 0);
		glUniform1f(shader_uniforms[shader_text_line][uniform_is_black], state.user_settings.black_map_font ? 1.f : 0.f);
		if((!state.cheat_data.province_names || zoom < map::zoom_very_close) && !text_line_vertices.empty()) {
			glUniform1f(shader_uniforms[shader_text_line][uniform_opaque], 0.f);
			glBindVertexArray(vao_array[vo_text_line]);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_array[vo_text_line]);
			for(uint32_t i = 0; i < uint32_t(text_line_texture_per_quad.size()); i++) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, text_line_texture_per_quad[i]);
				glDrawArrays(GL_TRIANGLES, i * 6, 6);
			}
		}
	}

	/*
		else if(state.cheat_data.province_names) {
			glUniform1f(15, 1.f);
			glBindVertexArray(vao_array[vo_province_text_line]);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_array[vo_province_text_line]);
			glDrawArrays(GL_TRIANGLES, 0, (GLsizei)province_text_line_vertices.size());
		}
	}*/

#if 0
	if(zoom > map::zoom_very_close && state.user_settings.render_models) {
		constexpr float dist_step = 1.77777f;
		// Render standing objects
		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);
		glClearDepth(-1.f);
		glDepthFunc(GL_GREATER);
		load_shader(shader_map_standing_object);
		glBindVertexArray(vao_array[vo_static_mesh]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_array[vo_static_mesh]);
		
		//for(uint32_t i = 0; i < uint32_t(static_mesh_starts.size()); i++) {
		//	glActiveTexture(GL_TEXTURE0);
		//	glBindTexture(GL_TEXTURE_2D, static_mesh_textures[i]);
		//	glUniform2f(12, 0.f, float(i * 8));
		//	glUniform1f(13, 0.f);
		//	glUniform1f(14, -0.75f);
		//	glDrawArrays(GL_TRIANGLES, static_mesh_starts[i], static_mesh_counts[i]);
		//}

		// Train stations
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, static_mesh_textures[5]);
		for(uint32_t i = 0; i < uint32_t(state.province_definitions.first_sea_province.index()); i++) {
			dcon::province_id p = dcon::province_id(dcon::province_id::value_base_t(i));
			auto const level = state.world.province_get_building_level(p, economy::province_building_type::railroad);
			if(level > 0) {
				auto center = state.world.province_get_mid_point(p);
				auto pos = center + glm::vec2(-dist_step, dist_step); //top right (from center)
				glUniform2f(shader_uniforms[shader_map_standing_object][uniform_model_offset], pos.x, pos.y);
				glUniform1f(shader_uniforms[shader_map_standing_object][uniform_target_facing], 2.f * glm::pi<float>() * (float(rng::reduce(p.index() * level, 1000)) / 1000.f));
				glUniform1f(shader_uniforms[shader_map_standing_object][uniform_target_topview_fixup], -0.75f);
				glDrawArrays(GL_TRIANGLES, static_mesh_starts[5], static_mesh_counts[5]);
			}
		}
		struct model_tier {
			uint32_t index;
			uint8_t min;
			uint8_t max;
		};
		// Naval base (empty)
		static const model_tier nbe_tiers[] = {
			{ 30, 1, 2 }, //early
			{ 32, 3, 4 }, //mid
			{ 6, 5, 6 } //late
		};
		for(const auto& tier : nbe_tiers) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, static_mesh_textures[tier.index]);
			for(uint32_t i = 0; i < uint32_t(state.province_definitions.first_sea_province.index()); i++) {
				dcon::province_id p = dcon::province_id(dcon::province_id::value_base_t(i));
				auto units = state.world.province_get_navy_location_as_location(p);
				auto const level = state.world.province_get_building_level(p, economy::province_building_type::naval_base);
				if(level >= tier.min && level <= tier.max && units.begin() == units.end()) {
					auto p1 = duplicates::get_navy_location(state, p);
					auto p2 = state.world.province_get_mid_point(p);
					glUniform2f(shader_uniforms[shader_map_standing_object][uniform_model_offset], p1.x, p1.y);
					auto theta = glm::atan(p2.y - p1.y, p2.x - p1.x);
					glUniform1f(shader_uniforms[shader_map_standing_object][uniform_target_facing], -theta);
					glUniform1f(shader_uniforms[shader_map_standing_object][uniform_target_topview_fixup], -0.75f);
					glDrawArrays(GL_TRIANGLES, static_mesh_starts[tier.index], static_mesh_counts[tier.index]);
				}
			}
		}
		// Naval base (full)
		static const model_tier nbf_tiers[] = {
			{ 31, 1, 2 }, //early
			{ 33, 3, 4 }, //mid
			{ 7, 5, 6 } //late
		};
		for(const auto& tier : nbf_tiers) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, static_mesh_textures[tier.index]);
			for(uint32_t i = 0; i < uint32_t(state.province_definitions.first_sea_province.index()); i++) {
				dcon::province_id p = dcon::province_id(dcon::province_id::value_base_t(i));
				auto units = state.world.province_get_navy_location_as_location(p);
				auto const level = state.world.province_get_building_level(p, economy::province_building_type::naval_base);
				if(level >= tier.min && level <= tier.max && units.begin() != units.end()) {
					auto p1 = duplicates::get_navy_location(state, p);
					auto p2 = state.world.province_get_mid_point(p);
					glUniform2f(shader_uniforms[shader_map_standing_object][uniform_model_offset], p1.x, p1.y);
					auto theta = glm::atan(p2.y - p1.y, p2.x - p1.x);
					glUniform1f(shader_uniforms[shader_map_standing_object][uniform_target_facing], -theta);
					glUniform1f(shader_uniforms[shader_map_standing_object][uniform_target_topview_fixup], -0.75f);
					glDrawArrays(GL_TRIANGLES, static_mesh_starts[tier.index], static_mesh_counts[tier.index]);
				}
			}
		}
		// Fort
		static const model_tier fort_tiers[] = {
			{ 8, 1, 2 }, //early
			{ 28, 3, 4 }, //mid
			{ 29, 5, 6 } //late
		};
		for(const auto& tier : fort_tiers) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, static_mesh_textures[tier.index]);
			for(uint32_t i = 0; i < uint32_t(state.province_definitions.first_sea_province.index()); i++) {
				dcon::province_id p = dcon::province_id(dcon::province_id::value_base_t(i));
				auto const level = state.world.province_get_building_level(p, economy::province_building_type::fort);
				if(level >= tier.min && level <= tier.max) {
					auto center = state.world.province_get_mid_point(p);
					auto pos = center + glm::vec2(dist_step, -dist_step); //bottom left (from center)
					glUniform2f(shader_uniforms[shader_map_standing_object][uniform_model_offset], pos.x, pos.y);
					glUniform1f(shader_uniforms[shader_map_standing_object][uniform_target_facing], 2.f * glm::pi<float>() * (float(rng::reduce(p.index(), 1000)) / 1000.f));
					glUniform1f(shader_uniforms[shader_map_standing_object][uniform_target_topview_fixup], -0.75f);
					glDrawArrays(GL_TRIANGLES, static_mesh_starts[tier.index], static_mesh_counts[tier.index]);
				}
			}
		}
		auto render_canal = [&](uint32_t index, uint32_t canal_id, float theta) {
			if(canal_id >= uint32_t(state.province_definitions.canals.size())
			&& canal_id >= uint32_t(state.province_definitions.canal_provinces.size()))
				return;
			auto const adj = state.province_definitions.canals[canal_id];
			if((state.world.province_adjacency_get_type(adj) & province::border::impassible_bit) != 0)
				return;
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, static_mesh_textures[index]);
			glm::vec2 pos = state.world.province_get_mid_point(state.province_definitions.canal_provinces[canal_id]);
			glUniform2f(shader_uniforms[shader_map_standing_object][uniform_model_offset], pos.x, pos.y);
			glUniform1f(shader_uniforms[shader_map_standing_object][uniform_target_facing], theta);
			glUniform1f(shader_uniforms[shader_map_standing_object][uniform_target_topview_fixup], 0.f);
			glDrawArrays(GL_TRIANGLES, static_mesh_starts[index], static_mesh_counts[index]);
		};
		render_canal(3, 0, 0.f); //Kiel
		render_canal(4, 1, glm::pi<float>() / 2.f), //Suez
		render_canal(2, 2, 0.f); //Panama
		// Factory
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, static_mesh_textures[9]);
		for(uint32_t i = 0; i < uint32_t(state.province_definitions.first_sea_province.index()); i++) {
			dcon::province_id p = dcon::province_id(dcon::province_id::value_base_t(i));
			auto factories = state.world.province_get_factory_location_as_province(p);
			if(factories.begin() != factories.end()) {
				auto center = state.world.province_get_mid_point(p);
				auto pos = center + glm::vec2(-dist_step, -dist_step); //top left (from center)
				glUniform2f(shader_uniforms[shader_map_standing_object][uniform_model_offset], pos.x, pos.y);
				glUniform1f(shader_uniforms[shader_map_standing_object][uniform_target_facing], 2.f * glm::pi<float>() * (float(rng::reduce(p.index(), 1000)) / 1000.f));
				glUniform1f(shader_uniforms[shader_map_standing_object][uniform_target_topview_fixup], -0.75f);
				glDrawArrays(GL_TRIANGLES, static_mesh_starts[9], static_mesh_counts[9]);
			}
		}
		// Blockaded
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, static_mesh_textures[10]);
		for(uint32_t i = 0; i < uint32_t(state.province_definitions.first_sea_province.index()); i++) {
			dcon::province_id p = dcon::province_id(dcon::province_id::value_base_t(i));
			if(military::province_is_blockaded(state, p)) {
				auto p1 = duplicates::get_navy_location(state, p);
				auto p2 = state.world.province_get_mid_point(p);
				glUniform2f(shader_uniforms[shader_map_standing_object][uniform_model_offset], p1.x, p1.y);
				auto theta = glm::atan(p2.y - p1.y, p2.x - p1.x);
				glUniform1f(shader_uniforms[shader_map_standing_object][uniform_target_facing], -theta);
				glUniform1f(shader_uniforms[shader_map_standing_object][uniform_target_topview_fixup], -0.75f);
				glDrawArrays(GL_TRIANGLES, static_mesh_starts[10], static_mesh_counts[10]);
			}
		}
		// Render armies
		auto render_regiment = [&](uint32_t index, military::unit_type type, float space) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, static_mesh_textures[index]);
			for(uint32_t i = 0; i < uint32_t(state.province_definitions.first_sea_province.index()); i++) {
				dcon::province_id p = dcon::province_id(dcon::province_id::value_base_t(i));
				auto units = state.world.province_get_army_location_as_location(p);
				if(state.map_state.visible_provinces[province::to_map_id(p)]
				&& units.begin() != units.end()) {
					auto p1 = state.world.province_get_mid_point(p);
					auto p2 = p1;
					bool has_unit = false;
					for(const auto unit : units) {
						for(const auto sm : unit.get_army().get_army_membership()) {
							auto& t = state.military_definitions.unit_base_definitions[sm.get_regiment().get_type()];
							if(t.type == type) {
								has_unit = true;
								break;
							}
						}
						if(has_unit) {
							if(auto path = unit.get_army().get_path(); path.size() > 0) {
								p2 = state.world.province_get_mid_point(path[path.size() - 1]);
								break;
							}
						}
					}
					if(has_unit) {
						glUniform2f(shader_uniforms[shader_map_standing_object][uniform_model_offset], p1.x, p1.y + space + dist_step);
						auto theta = glm::atan(p2.y - p1.y, p2.x - p1.x);
						glUniform1f(shader_uniforms[shader_map_standing_object][uniform_target_facing], -theta);
						glUniform1f(shader_uniforms[shader_map_standing_object][uniform_target_topview_fixup], -0.75f);
						glDrawArrays(GL_TRIANGLES, static_mesh_starts[index], static_mesh_counts[index]);
					}
				}
			}
		};
		render_regiment(17, military::unit_type::infantry, dist_step); //shadow
		render_regiment(17, military::unit_type::cavalry, 0.f); //shadow
		render_regiment(17, military::unit_type::support, -dist_step); //shadow
		render_regiment(11, military::unit_type::infantry, dist_step); //infantry
		render_regiment(15, military::unit_type::cavalry, 0.f); //horse
		render_regiment(18, military::unit_type::support, -dist_step); //artillery
		// Render navies
		auto render_ship = [&](uint32_t index, military::unit_type type, int32_t min_port, float space) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, static_mesh_textures[index]);
			for(uint32_t i = uint32_t(state.province_definitions.first_sea_province.index()); i < state.world.province_size(); i++) {
				dcon::province_id p = dcon::province_id(dcon::province_id::value_base_t(i));
				auto units = state.world.province_get_navy_location_as_location(p);
				if(state.map_state.visible_provinces[province::to_map_id(p)]
				&& units.begin() != units.end()) {
					auto p1 = duplicates::get_navy_location(state, p);
					auto p2 = p1;
					bool has_unit = false;
					for(const auto unit : units) {
						for(const auto sm : unit.get_navy().get_navy_membership()) {
							auto& t = state.military_definitions.unit_base_definitions[sm.get_ship().get_type()];
							if(t.type == type && t.min_port_level <= min_port) {
								has_unit = true;
								break;
							}
						}
						if(has_unit) {
							if(auto path = unit.get_navy().get_path(); path.size() > 0) {
								p2 = duplicates::get_navy_location(state, path[path.size() - 1]);
								break;
							}
						}
					}
					if(has_unit) {
						glUniform2f(shader_uniforms[shader_map_standing_object][uniform_model_offset], p1.x, p1.y + space + dist_step);
						auto theta = glm::atan(p2.y - p1.y, p2.x - p1.x);
						glUniform1f(shader_uniforms[shader_map_standing_object][uniform_target_facing], -theta);
						glUniform1f(shader_uniforms[shader_map_standing_object][uniform_target_topview_fixup], -0.75f);
						glDrawArrays(GL_TRIANGLES, static_mesh_starts[index], static_mesh_counts[index]);
					}
				}
			}
		};

		//ship wakes
		render_ship(16, military::unit_type::big_ship, 2, 3.f * dist_step); //raider
		render_ship(16, military::unit_type::transport, -1, 2.f * dist_step); //transport
		render_ship(16, military::unit_type::big_ship, -1, dist_step); //manowar
		render_ship(16, military::unit_type::light_ship, -1, 0.f); //frigate
		render_ship(16, military::unit_type::big_ship, 4, -dist_step); //battleship
		render_ship(16, military::unit_type::light_ship, 3, -2.f * dist_step); //cruiser
		render_ship(16, military::unit_type::big_ship, 3, -3.f * dist_step); //ironclad
		//ship themselves
		render_ship(37, military::unit_type::big_ship, 2, 3.f * dist_step); //raider
		render_ship(14, military::unit_type::transport, -1, 2.f * dist_step); //transport
		render_ship(13, military::unit_type::big_ship, -1, dist_step); //manowar
		render_ship(12, military::unit_type::light_ship, -1, 0.f); //frigate
		render_ship(34, military::unit_type::big_ship, 4, -dist_step); //battleship
		render_ship(35, military::unit_type::light_ship, 3, -2.f * dist_step); //cruiser
		render_ship(36, military::unit_type::big_ship, 3, -3.f * dist_step); //ironclad

		// Floating flags
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, static_mesh_textures[38]);
		for(uint32_t i = 0; i < state.world.province_size(); i++) {
			dcon::province_id p = dcon::province_id(dcon::province_id::value_base_t(i));
			auto units = state.world.province_get_navy_location_as_location(p);
			if(state.map_state.visible_provinces[province::to_map_id(p)]
			&& units.begin() != units.end()) {
				auto p1 = state.world.province_get_mid_point(p);
				glUniform2f(shader_uniforms[shader_map_standing_object][uniform_model_offset], p1.x, p1.x);
				auto theta = glm::atan(0.f, 0.f);
				glUniform1f(shader_uniforms[shader_map_standing_object][uniform_target_facing], -theta);
				glUniform1f(shader_uniforms[shader_map_standing_object][uniform_target_topview_fixup], -0.75f);
				glDrawArrays(GL_TRIANGLES, static_mesh_starts[38], static_mesh_counts[38]);
			}
		}
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, static_mesh_textures[39]);
		for(uint32_t i = 0; i < state.world.province_size(); i++) {
			dcon::province_id p = dcon::province_id(dcon::province_id::value_base_t(i));
			auto units = state.world.province_get_navy_location_as_location(p);
			if(state.map_state.visible_provinces[province::to_map_id(p)]
			&& units.begin() != units.end()) {
				auto p1 = state.world.province_get_mid_point(p);
				glUniform2f(shader_uniforms[shader_map_standing_object][uniform_model_offset], p1.x, p1.x);
				auto theta = glm::atan(0.f, 0.f);
				glUniform1f(shader_uniforms[shader_map_standing_object][uniform_target_facing], -theta);
				glUniform1f(shader_uniforms[shader_map_standing_object][uniform_target_topview_fixup], -0.75f);
				glDrawArrays(GL_TRIANGLES, static_mesh_starts[39], static_mesh_counts[39]);
			}
		}

		for(uint32_t i = 0; i < 3 * 3; i++) {
			auto index = 19 + i;
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, static_mesh_textures[index]);
			for(uint32_t j = 0; j < uint32_t(state.province_definitions.first_sea_province.index()); j++) {
				dcon::province_id p = dcon::province_id(dcon::province_id::value_base_t(j));
				if(state.world.province_get_demographics(p, demographics::total) >= float(i * 100000.f)) {
					// 1 2 3 -- 1
					// 1 2 3 -- 2
					// 1 2 3 -- 3
					auto center = state.world.province_get_mid_point(p);
					auto pos = center;
					pos.x += i == 0 ? 0.f : float(i % 3) * 2.5f;
					pos.y += i == 0 ? 0.f : float(i / 3) * 2.5f;
					pos.x -= 2.5f;
					pos.y -= 2.5f;
					glUniform2f(shader_uniforms[shader_map_standing_object][uniform_model_offset], pos.x, pos.y);
					glUniform1f(shader_uniforms[shader_map_standing_object][uniform_target_facing], 2.f * glm::pi<float>() * (float(rng::reduce(p.index() + j + i, 1000)) / 1000.f));
					glUniform1f(shader_uniforms[shader_map_standing_object][uniform_target_topview_fixup], -0.75f);
					glDrawArrays(GL_TRIANGLES, static_mesh_starts[index], static_mesh_counts[index]);
					glUniform2f(shader_uniforms[shader_map_standing_object][uniform_model_offset], pos.x, pos.y);
					glUniform1f(shader_uniforms[shader_map_standing_object][uniform_target_facing], 2.f * glm::pi<float>() * (float(rng::reduce(p.index() + j * i, 1000)) / 1000.f));
					glUniform1f(shader_uniforms[shader_map_standing_object][uniform_target_topview_fixup], -0.75f);
					glDrawArrays(GL_TRIANGLES, static_mesh_starts[index], static_mesh_counts[index]);
				}
			}
		}
		glDisable(GL_DEPTH_TEST);
	}
#endif

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
		glUniform1f(state.open_gl.msaa_uniform_gaussian_blur, state.user_settings.gaussianblur_level);
		glUniform2f(state.open_gl.msaa_uniform_screen_size, screen_size.x, screen_size.y);
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
		ogl::set_gltex_parameters(texture_handle, GL_TEXTURE_2D, GL_NEAREST, GL_CLAMP_TO_EDGE);
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
	std::vector<uint32_t> province_highlights(state.world.province_size() + 1, 0);
	state.current_scene.update_highlight_texture(state, province_highlights, prov_id);
	gen_prov_color_texture(textures[texture_province_highlight], province_highlights);
}

void display_data::set_province_color(std::vector<uint32_t> const& prov_color) {
	gen_prov_color_texture(texture_arrays[texture_array_province_color], prov_color, 2);
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

	glBindBuffer(GL_ARRAY_BUFFER, vbo_array[vo_drag_box]);
	assert(!drag_box_vertices.empty());
	glBufferData(GL_ARRAY_BUFFER, sizeof(screen_vertex) * drag_box_vertices.size(), &drag_box_vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void add_arrow_to_buffer(std::vector<map::curved_line_vertex>& buffer, glm::vec2 start, glm::vec2 end, glm::vec2 prev_normal_dir, glm::vec2 next_normal_dir, float fill_progress, bool end_arrow, float size_x, float size_y) {
	constexpr float type_filled = 2.f;
	constexpr float type_unfilled = 0.f;
	constexpr float type_end = 3.f;
	glm::vec2 curr_dir = glm::normalize(end - start);
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
	auto d = start - end;
	d.x *= 2.0f;
	distance += 0.5f * glm::length(d);
	buffer.emplace_back(textured_line_vertex{ end, +next_normal_dir, 0.0f, distance });//C
	buffer.emplace_back(textured_line_vertex{ end, -next_normal_dir, 1.0f, distance });//D
}

void add_tl_segment_buffer(
	std::vector<map::textured_line_with_width_vertex>& buffer,
	glm::vec2 start,
	glm::vec2 end,
	glm::vec2 next_normal_dir,
	float size_x,
	float size_y,
	float& distance,
	float width
) {
	start /= glm::vec2(size_x, size_y);
	end /= glm::vec2(size_x, size_y);
	auto d = start - end;
	distance += glm::length(d) * width / 1000.f;
	buffer.emplace_back(textured_line_with_width_vertex{ end, +next_normal_dir, 0.0f, distance, width});//C
	buffer.emplace_back(textured_line_with_width_vertex{ end, -next_normal_dir, 1.0f, distance, width});//D
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

void add_tl_bezier_to_buffer(
	std::vector<map::textured_line_with_width_vertex>& buffer,
	glm::vec2 start,
	glm::vec2 end,
	glm::vec2 start_tangent,
	glm::vec2 end_tangent,
	float progress,
	bool last_curve,
	float size_x,
	float size_y,
	uint32_t num_b_segments,
	float& distance,
	float width_start,
	float width_end
) {
	auto control_point_length = glm::length(end - start) * control_point_length_factor;

	//auto start_normal = -glm::vec2{ start_tangent.y, -start_tangent.x };
	//auto end_normal = -glm::vec2{ end_tangent.y, -end_tangent.x };

	//auto start_control_point = start_normal * control_point_length + start;
	//auto end_control_point = -end_normal * control_point_length + end;

	auto start_control_point = start_tangent * control_point_length + start;
	auto end_control_point = end_tangent * control_point_length + end;

	auto bpoint = [=](float t) {
		auto u = 1.0f - t;
		return 0.0f
			+ (u * u * u) * start
			+ (3.0f * u * u * t) * start_control_point
			+ (3.0f * u * t * t) * end_control_point
			+ (t * t * t) * end;
		};

	auto last_normal = glm::vec2(-start_tangent.y, start_tangent.x);
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

		auto width = t_start * width_end + (1.f - t_start) * width_start;

		if(width != width_end) {
			auto help = true;
		}

		add_tl_segment_buffer(buffer, start_point, end_point, next_normal, size_x, size_y, distance, width);

		last_normal = next_normal;
	}
	{
		next_normal = glm::vec2(end_tangent.y, -end_tangent.x);
		auto t_start = float(num_b_segments - 1) / float(num_b_segments);
		auto t_end = 1.0f;
		auto start_point = bpoint(t_start);
		auto end_point = bpoint(t_end);
		auto width = t_start * width_end + (1.f - t_start) * width_start;

		add_tl_segment_buffer(buffer, start_point, end_point, next_normal, size_x, size_y, distance, width);
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

void make_navy_direction(sys::state& state, std::vector<map::curved_line_vertex>& buffer, dcon::navy_id selected_navy, float size_x, float size_y) {
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
		for(auto i = ps; i-- > path.size() - 1;) {
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

			add_bezier_to_buffer(buffer, current_pos, next_pos, prev_perpendicular, next_perpendicular, i == ps - 1 ? progress : 0.0f, i == path.size() - 1, size_x, size_y, default_num_b_segments);

			prev_perpendicular = -1.0f * next_perpendicular;
			current_pos = duplicates::get_navy_location(state, path[i]);
		}
	}
}
void make_sea_path(
	sys::state& state,
	std::vector<map::textured_line_with_width_vertex>& buffer,
	dcon::province_id origin,
	dcon::province_id target,
	float width,
	float size_x,
	float size_y,
	float shift_x,
	float shift_y
) {
	auto path = province::make_naval_path(state, origin, target);
	float distance = 0.0f;

	auto shift = glm::vec2(shift_x, shift_y) / glm::vec2(size_x, size_y);
	if(auto ps = path.size(); ps > 0) {
		glm::vec2 current_pos = duplicates::get_army_location(state, origin) + shift;
		glm::vec2 next_pos = put_in_local(duplicates::get_army_location(state, path[ps - 1]), current_pos, size_x);
		glm::vec2 prev_perpendicular = glm::normalize(next_pos - current_pos);
		auto start_normal = glm::vec2(-prev_perpendicular.y, prev_perpendicular.x);
		auto norm_pos = current_pos / glm::vec2(size_x, size_y);

		buffer.emplace_back(map::textured_line_with_width_vertex {
			norm_pos,
			+start_normal,
			0.f,
			0.0f,
			width
		});
		buffer.emplace_back(map::textured_line_with_width_vertex {
			norm_pos,
			-start_normal,
			1.f,
			0.0f,
			width
		});

		for(auto i = ps; i-- > 0;) {
			glm::vec2 next_perpendicular{ 0.0f, 0.0f };
			next_pos = put_in_local(duplicates::get_army_location(state, path[i]) + shift, current_pos, size_x);

			if(i > 0) {
				glm::vec2 next_next_pos = put_in_local(duplicates::get_army_location(state, path[i - 1]) + shift, next_pos, size_x);
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

			add_tl_bezier_to_buffer(
				buffer,
				current_pos,
				next_pos,
				prev_perpendicular,
				next_perpendicular,
				0.0f,
				i == 0,
				size_x,
				size_y,
				default_num_b_segments,
				distance,
				width,
				width
			);

			prev_perpendicular = -1.0f * next_perpendicular;
			current_pos = duplicates::get_army_location(state, path[i]) + shift;
		}
	}
}

void make_land_path(
	sys::state& state,
	std::vector<map::textured_line_with_width_vertex>& buffer,
	dcon::province_id origin,
	dcon::province_id target,
	float width,
	float size_x,
	float size_y
) {
	auto path = province::make_unowned_land_path(state, origin, target);
	float distance = 0.0f;
	if(auto ps = path.size(); ps > 0) {
		glm::vec2 current_pos = duplicates::get_army_location(state, origin);
		glm::vec2 next_pos = put_in_local(duplicates::get_army_location(state, path[ps - 1]), current_pos, size_x);
		glm::vec2 prev_tangent = glm::normalize(next_pos - current_pos);
		auto start_normal = glm::vec2(-prev_tangent.y, prev_tangent.x);
		auto norm_pos = current_pos / glm::vec2(size_x, size_y);

		buffer.emplace_back(map::textured_line_with_width_vertex{
			norm_pos,
			+start_normal,
			0.f,
			0.0f,
			width
		});
		buffer.emplace_back(map::textured_line_with_width_vertex{
			norm_pos,
			-start_normal,
			1.f,
			0.0f,
			width
		});

		for(auto i = ps; i-- > 0;) {
			glm::vec2 next_tangent{ 0.0f, 0.0f };
			next_pos = put_in_local(duplicates::get_army_location(state, path[i]), current_pos, size_x);

			if(i > 0) {
				glm::vec2 next_next_pos = put_in_local(duplicates::get_army_location(state, path[i - 1]), next_pos, size_x);
				glm::vec2 a_per = glm::normalize(next_pos - current_pos);
				glm::vec2 b_per = glm::normalize(next_pos - next_next_pos);
				glm::vec2 temp = a_per + b_per;
				if(glm::length(temp) < 0.00001f) {
					next_tangent = -a_per;
				} else {
					next_tangent = glm::normalize(glm::vec2{ -temp.y, temp.x });
					if(glm::dot(a_per, -next_tangent) < glm::dot(a_per, next_tangent)) {
						next_tangent *= -1.0f;
					}
				}
			} else {
				next_tangent = glm::normalize(current_pos - next_pos);
			}

			add_tl_bezier_to_buffer(
				buffer,
				current_pos,
				next_pos,
				prev_tangent,
				next_tangent,
				0.0f,
				i == 0,
				size_x,
				size_y,
				default_num_b_segments,
				distance,
				width,
				width
			);

			//prev_perpendicular = -1.0f * next_perpendicular;
			prev_tangent = -next_tangent;
			current_pos = duplicates::get_army_location(state, path[i]);
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

void make_army_direction(sys::state& state, std::vector<map::curved_line_vertex>& buffer, dcon::army_id selected_army, float size_x, float size_y) {
	auto path = state.world.army_get_path(selected_army);
	if(auto ps = path.size(); ps > 0) {
		auto progress = military::fractional_distance_covered(state, selected_army);

		glm::vec2 current_pos = duplicates::get_army_location(state, state.world.army_get_location_from_army_location(selected_army));
		glm::vec2 next_pos = put_in_local(duplicates::get_army_location(state, path[ps - 1]), current_pos, size_x);
		glm::vec2 prev_perpendicular = glm::normalize(next_pos - current_pos);

		auto start_normal = glm::vec2(-prev_perpendicular.y, prev_perpendicular.x);
		auto norm_pos = current_pos / glm::vec2(size_x, size_y);

		buffer.emplace_back(norm_pos, +start_normal, glm::vec2{ 0,0 }, glm::vec2(0.0f, 0.0f), progress > 0.0f ? 2.0f : 0.0f);
		buffer.emplace_back(norm_pos, -start_normal, glm::vec2{ 0,0 }, glm::vec2(0.0f, 1.0f), progress > 0.0f ? 2.0f : 0.0f);
		for(auto i = ps; i-- > path.size()-1;) {
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

			add_bezier_to_buffer(buffer, current_pos, next_pos, prev_perpendicular, next_perpendicular, i == ps - 1 ? progress : 0.0f, i == path.size() - 1, size_x, size_y, default_num_b_segments);

			prev_perpendicular = -1.0f * next_perpendicular;
			current_pos = duplicates::get_army_location(state, path[i]);
		}
	}
}

void create_railroad_connection(sys::state& state, std::vector<glm::vec2>& railroad, dcon::province_id p1, dcon::province_id p2) {
	auto const m1 = state.world.province_get_mid_point(p1);
	auto const m2 = state.world.province_get_mid_point(p2);
	railroad.emplace_back(m1);
	//
	auto mid = ((m1 + m2) / glm::vec2(2.f, 2.f));
	const auto rp = rng::get_random(state, p1.index(), p2.index() ^ p1.index());
	const float rf = float(rng::reduce(uint32_t(rp), 8192)) / (8192.f * 0.25f);
	auto const perpendicular = glm::normalize(m2 - m1) * glm::vec2(rf, rf);
	railroad.emplace_back(mid + glm::vec2(-perpendicular.y, perpendicular.x));
}

bool get_provinces_part_of_rr_path(sys::state& state, std::vector<bool>& visited_adj, std::vector<bool>& visited_prov, std::vector<dcon::province_id>& provinces, dcon::province_id p) {
	if(state.world.province_get_building_level(p, uint8_t(economy::province_building_type::railroad)) == 0)
		return false;
	if(!p)
		return false;
	if(visited_prov[p.index()])
		return false;
	visited_prov[p.index()] = true;
	provinces.push_back(p);

	std::vector<dcon::province_adjacency_id> valid_adj;
	for(const auto adj : state.world.province_get_province_adjacency_as_connected_provinces(p)) {
		auto const pa = adj.get_connected_provinces(adj.get_connected_provinces(0) == p ? 1 : 0);
		if(pa.get_building_level(uint8_t(economy::province_building_type::railroad)) == 0
			|| visited_prov[pa.id.index()])
			continue;
		// Do not display railroads if it's a strait OR an impassable land border!
		if((adj.get_type() & province::border::impassible_bit) != 0
			|| (adj.get_type() & province::border::non_adjacent_bit) != 0)
			continue;
		assert(adj.id);
		valid_adj.push_back(adj.id);
	}
	std::sort(valid_adj.begin(), valid_adj.end(), [&](auto const a, auto const b) -> bool {
		auto const ad = state.world.province_adjacency_get_distance(a);
		auto const bd = state.world.province_adjacency_get_distance(b);
		return ad < bd;
	});
	for(const auto a : valid_adj) {
		auto const adj = dcon::fatten(state.world, a);
		auto const pa = adj.get_connected_provinces(adj.get_connected_provinces(0) == p ? 1 : 0);
		if(get_provinces_part_of_rr_path(state, visited_adj, visited_prov, provinces, pa.id)) {
			visited_adj[adj.id.index()] = true;
			break;
		}
	}
	return true;
}

void display_data::update_railroad_paths(sys::state& state) {
	// Create paths for the main railroad sections
	std::vector<bool> visited_prov(state.world.province_size() + 1, false);
	std::vector<bool> rr_ends(state.world.province_size() + 1, false);
	std::vector<bool> visited_adj(state.world.province_adjacency_size() + 1, false);
	std::vector<std::vector<glm::vec2>> railroads;
	for(const auto p : state.world.in_province) {
		std::vector<dcon::province_id> provinces;
		if(get_provinces_part_of_rr_path(state, visited_adj, visited_prov, provinces, p)) {
			if(provinces.size() > 1) {
				std::vector<glm::vec2> railroad;
				for(uint32_t i = 0; i < uint32_t(provinces.size() - 1); i++)
					create_railroad_connection(state, railroad, provinces[i], provinces[i + 1]);
				railroad.emplace_back(state.world.province_get_mid_point(provinces.back()));
				assert(!railroad.empty());
				railroads.push_back(railroad);
				assert(provinces.front());
				assert(provinces.back());
				rr_ends[provinces.front().index()] = true;
				rr_ends[provinces.back().index()] = true;
			}
		}
	}

	// Populate paths with railroads - only account provinces that have been visited
	// but not the adjacencies
	for(const auto p1 : state.world.in_province) {
		if(visited_prov[p1.id.index()]) {
			auto const p1_level = p1.get_building_level(uint8_t(economy::province_building_type::railroad));
			auto admin_efficiency = state.world.province_get_control_ratio(p1);
			auto max_adj = std::max<uint32_t>(uint32_t(admin_efficiency * 2.75f), rr_ends[p1.id.index()] ? 3 : 1);
			std::vector<dcon::province_adjacency_id> valid_adj;
			for(const auto adj : p1.get_province_adjacency_as_connected_provinces()) {
				if(max_adj == 0)
					break;
				auto p2 = adj.get_connected_provinces(adj.get_connected_provinces(0) == p1.id ? 1 : 0);
				if(p2.get_building_level(uint8_t(economy::province_building_type::railroad)) == 0)
					continue;
				max_adj--;
				assert(adj.id);
				assert(p2);
				if(visited_adj[adj.id.index()])
					continue;
				if(rr_ends[p1.id.index()] != rr_ends[p2.id.index()]
				&& rr_ends[p1.id.index()] == false)
					continue;
				visited_adj[adj.id.index()] = true;
				valid_adj.push_back(adj.id);
			}
			std::sort(valid_adj.begin(), valid_adj.end(), [&](auto const a, auto const b) -> bool {
				auto const ad = state.world.province_adjacency_get_distance(a);
				auto const bd = state.world.province_adjacency_get_distance(b);
				return ad < bd;
			});
			for(const auto a : valid_adj) {
				auto const adj = dcon::fatten(state.world, a);
				auto const p2 = adj.get_connected_provinces(adj.get_connected_provinces(0) == p1.id ? 1 : 0);
				//
				std::vector<glm::vec2> railroad;
				create_railroad_connection(state, railroad, p1.id, p2.id);
				railroad.emplace_back(state.world.province_get_mid_point(p2.id));
				assert(!railroad.empty());
				railroads.push_back(railroad);
			}
		}
	}

	railroad_vertices.clear();
	railroad_starts.clear();
	railroad_counts.clear();
	for(const auto& railroad : railroads) {
		railroad_starts.push_back(GLint(railroad_vertices.size()));
		glm::vec2 current_pos = railroad.back();
		glm::vec2 next_pos = put_in_local(railroad[railroad.size() - 2], current_pos, float(size_x));
		glm::vec2 prev_perpendicular = glm::normalize(next_pos - current_pos);
		auto start_normal = glm::vec2(-prev_perpendicular.y, prev_perpendicular.x);
		auto norm_pos = current_pos / glm::vec2(size_x, size_y);
		railroad_vertices.emplace_back(textured_line_vertex{ norm_pos, +start_normal, 0.0f, 0.f });//C
		railroad_vertices.emplace_back(textured_line_vertex{ norm_pos, -start_normal, 1.0f, 0.f });//D
		float distance = 0.0f;
		for(auto i = railroad.size() - 1; i-- > 0;) {
			glm::vec2 next_perpendicular{ 0.0f, 0.0f };
			next_pos = put_in_local(railroad[i], current_pos, float(size_x));
			if(i > 0) {
				glm::vec2 next_next_pos = put_in_local(railroad[i - 1], next_pos, float(size_x));
				glm::vec2 a_per = glm::normalize(next_pos - current_pos);
				glm::vec2 b_per = glm::normalize(next_pos - next_next_pos);
				glm::vec2 temp = a_per + b_per;
				if(glm::length(temp) < 0.00001f) {
					next_perpendicular = -a_per;
				} else {
					next_perpendicular = glm::normalize(glm::vec2{ -temp.y, temp.x });
					if(glm::dot(a_per, -next_perpendicular) < glm::dot(a_per, next_perpendicular))
						next_perpendicular *= -1.0f;
				}
			} else {
				next_perpendicular = glm::normalize(current_pos - next_pos);
			}
			add_tl_bezier_to_buffer(railroad_vertices, current_pos, next_pos, prev_perpendicular, next_perpendicular, 0.0f, false, float(size_x), float(size_y), default_num_b_segments, distance);
			prev_perpendicular = -1.0f * next_perpendicular;
			current_pos = railroad[i];
		}
		railroad_counts.push_back(GLsizei(railroad_vertices.size() - railroad_starts.back()));
		assert(railroad_counts.back() > 1);
	}
	assert(railroad_counts.size() == railroad_starts.size());

	if(!railroad_vertices.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo_array[vo_railroad]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(textured_line_vertex) * railroad_vertices.size(), railroad_vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void display_data::set_text_lines(sys::state& state, std::vector<text_line_generator_data> const& data) {
	text_line_vertices.clear();
	text_line_texture_per_quad.clear();

	const auto map_x_scaling = float(size_x) / float(size_y);
	auto& f = state.font_collection.get_font(state, text::font_selection::map_font);

	for(const auto& e : data) {
		// omit invalid, nan or infinite coefficients
		if(!std::isfinite(e.coeff[0]) || !std::isfinite(e.coeff[1]) || !std::isfinite(e.coeff[2]) || !std::isfinite(e.coeff[3]))
			continue;

		bool is_linear = true;
		if((e.coeff[2] != 0) || (e.coeff[3] != 0)) {
			is_linear = false;
		}

		// y = a + bx + cx^2 + dx^3
		// y = mo[0] + mo[1] * x + mo[2] * x * x + mo[3] * x * x * x
		auto poly_fn = [&](float x) {
			return e.coeff[0] + e.coeff[1] * x + e.coeff[2] * x * x + e.coeff[3] * x * x * x;
			};
		auto dpoly_fn = [&](float x) {
			// y = a + 1bx^1 + 1cx^2 + 1dx^3
			// y = 0 + 1bx^0 + 2cx^1 + 3dx^2
			return e.coeff[1] + 2.f * e.coeff[2] * x + 3.f * e.coeff[3] * x * x;
		};


		//cutting box if graph goes outside

		float left = 0.f;
		float right = 1.f;

		if(is_linear) {
			if(e.coeff[1] > 0.01f) {
				left = (-e.coeff[0]) / e.coeff[1];
				right = (1.f - e.coeff[0]) / e.coeff[1];
			} else if(e.coeff[1] < -0.01f) {
				left = (1.f - e.coeff[0]) / e.coeff[1];
				right = (- e.coeff[0]) / e.coeff[1];
			}
		} else {
			while(((poly_fn(left) < 0.f) || (poly_fn(left) > 1.f)) && (left < 1.f)) {
				left += 1.f / 300.f;
			}
			while(((poly_fn(right) < 0.f) || (poly_fn(right) > 1.f)) && (right > 0.f)) {
				right -= 1.f / 300.f;
			}
		}


		left = std::clamp(left, 0.f, 1.f);
		right = std::clamp(right, 0.f, 1.f);


		if(right <= left) {
			continue;
		}

		float result_interval = right - left;
		float center = (right + left) / 2.f;

		glm::vec2 ratio = e.ratio;
		glm::vec2 basis = e.basis;

		auto effective_ratio = ratio.x * map_x_scaling / ratio.y;

		float text_length = f.text_extent(state, e.text, 0, uint32_t(e.text.glyph_info.size()), 1);
		assert(std::isfinite(text_length) && text_length != 0.f);
		float x_step = (result_interval / float(e.text.glyph_info.size() * 32.f));
		float curve_length = 0.f; //width of whole string polynomial
		if(is_linear) {
			float height = poly_fn(right) - poly_fn(left);
			curve_length = 2.f * glm::length(glm::vec2(height * ratio.y, result_interval * ratio.x));
		} else for(float x = left; x <= right; x += x_step) {
			curve_length += 2.0f * glm::length(glm::vec2(x_step * ratio.x, (poly_fn(x) - poly_fn(x + x_step)) * ratio.y));
		}
		float size = (curve_length / text_length) * 0.8f; //* 0.66f;

		// typography "golden ratio" steps

		float font_size_index = std::round(5.f * log(size) / log(1.618034f));

		if(font_size_index > 45.f) {
			font_size_index = 45.f;
		}
		if (font_size_index > 5.f)
			font_size_index = 5.f * std::round(font_size_index / 5.f);

		size = std::pow(1.618034f, font_size_index / 5.f);

		// fixed step

		/*
		float size_step = 30.f;

		if(size > size_step * 6.f) {
			size = size_step * 6.f; //+ (size - 200.0f) * 0.5f;
		}

		if(size > ratio.x / 2.f) {
			size = ratio.x / 2.f;
		}
		if(size > ratio.y / 2.f) {
			size = ratio.y / 2.f;
		}
		
		size = std::round(size / size_step) * size_step;

		if(size < size_step) {
			continue;
		}
		*/

		auto real_text_size = size / (size_x * 2.0f);

		float letter_spacing_map = std::clamp((0.8f * curve_length / text_length - size) / 2.f, 0.f, size * 2.f);
		if(state.world.locale_get_prevent_letterspace(state.font_collection.get_current_locale())) {
			letter_spacing_map = 0.f;
		}

		float margin = (curve_length - text_length * (size + letter_spacing_map * 2.f) + letter_spacing_map) / 2.0f;
		float x = left;
		for(float accumulated_length = 0.f; ; x += x_step) {
			auto added_distance = 2.0f * glm::length(glm::vec2(x_step * ratio.x, (poly_fn(x) - poly_fn(x + x_step)) * e.ratio.y));
			if(accumulated_length + added_distance >= margin) {
				x += x_step * (margin - accumulated_length) / added_distance;
				break;
			}
			accumulated_length += added_distance;
		}


		unsigned int glyph_count = static_cast<unsigned int>(e.text.glyph_info.size());
		for(unsigned int i = 0; i < glyph_count; i++) {
			hb_codepoint_t glyphid = e.text.glyph_info[i].codepoint;
			auto gso = f.glyph_positions[glyphid];
			float x_advance = float(e.text.glyph_info[i].x_advance) / (float((1 << 6) * text::magnification_factor));
			float x_offset = float(e.text.glyph_info[i].x_offset) / (float((1 << 6) * text::magnification_factor)) + float(gso.x);
			float y_offset = float(gso.y) - float(e.text.glyph_info[i].y_offset) / (float((1 << 6) * text::magnification_factor));
			if(glyphid != FT_Get_Char_Index(f.font_face, ' ')) {
				// Add up baseline and kerning offsets
				glm::vec2 glyph_positions{ x_offset / 64.f, -y_offset / 64.f };

				glm::vec2 curr_dir = glm::normalize(glm::vec2(effective_ratio, dpoly_fn(x)));
				glm::vec2 curr_normal_dir = glm::vec2(-curr_dir.y, curr_dir.x);
				curr_dir.x *= 0.5f;
				curr_normal_dir.x *= 0.5f;

				glm::vec2 shader_direction = glm::normalize(glm::vec2(ratio.x, dpoly_fn(x) * ratio.y));

				auto p0 = glm::vec2(x, poly_fn(x)) * ratio + basis;
				p0 /= glm::vec2(size_x, size_y); // Rescale the coordinate to 0-1
				p0 -= (1.5f - 2.f * glyph_positions.y) * curr_normal_dir * real_text_size;
				p0 += (1.0f + 2.f * glyph_positions.x) * curr_dir * real_text_size;

				float type = float((gso.texture_slot >> 6) % text::max_texture_layers);
				float step = 1.f / 8.f;
				float tx = float(gso.texture_slot & 7) * step;
				float ty = float((gso.texture_slot & 63) >> 3) * step;

				text_line_vertices.emplace_back(p0, glm::vec2(-1, 1), shader_direction, glm::vec3(tx, ty, type), real_text_size);
				text_line_vertices.emplace_back(p0, glm::vec2(-1, -1), shader_direction, glm::vec3(tx, ty + step, type), real_text_size);
				text_line_vertices.emplace_back(p0, glm::vec2(1, -1), shader_direction, glm::vec3(tx + step, ty + step, type), real_text_size);

				text_line_vertices.emplace_back(p0, glm::vec2(1, -1), shader_direction, glm::vec3(tx + step, ty + step, type), real_text_size);
				text_line_vertices.emplace_back(p0, glm::vec2(1, 1), shader_direction, glm::vec3(tx + step, ty, type), real_text_size);
				text_line_vertices.emplace_back(p0, glm::vec2(-1, 1), shader_direction, glm::vec3(tx, ty, type), real_text_size);
				text_line_texture_per_quad.emplace_back(f.textures[gso.texture_slot >> 6]);
			}
			float glyph_advance = x_advance * size / 64.f;
			for(float glyph_length = 0.f; ; x += x_step) {
				auto added_distance = 2.0f * glm::length(glm::vec2(x_step * ratio.x, (poly_fn(x) - poly_fn(x + x_step)) * ratio.y));
				if(glyph_length + added_distance >= glyph_advance + letter_spacing_map) {
					x += x_step * (glyph_advance + letter_spacing_map - glyph_length) / added_distance;
					break;
				}
				glyph_length += added_distance;
			}
		}
	}
	if(text_line_vertices.size() > 0) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo_array[vo_text_line]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(text_line_vertex) * text_line_vertices.size(), &text_line_vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void display_data::set_province_text_lines(sys::state& state, std::vector<text_line_generator_data> const& data) {
	province_text_line_vertices.clear();
	const auto map_x_scaling = float(size_x) / float(size_y);
	auto& f = state.font_collection.get_font(state, text::font_selection::map_font);
	
	for(const auto& e : data) {
		// omit invalid, nan or infinite coefficients
		if(!std::isfinite(e.coeff[0]) || !std::isfinite(e.coeff[1]) || !std::isfinite(e.coeff[2]) || !std::isfinite(e.coeff[3]))
			continue;

		auto effective_ratio = e.ratio.x * map_x_scaling / e.ratio.y;

		float text_length = f.text_extent(state, e.text, 0, uint32_t(e.text.glyph_info.size()), 1);
		assert(std::isfinite(text_length) && text_length != 0.f);
		// y = a + bx + cx^2 + dx^3
		// y = mo[0] + mo[1] * x + mo[2] * x * x + mo[3] * x * x * x
		auto poly_fn = [&](float x) {
			return e.coeff[0] + e.coeff[1] * x + e.coeff[2] * x * x + e.coeff[3] * x * x * x;
			};
		float x_step = (1.f / float(e.text.glyph_info.size() * 32.f));
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

		unsigned int glyph_count = uint32_t(e.text.glyph_info.size());
		for(unsigned int i = 0; i < glyph_count; i++) {
			hb_codepoint_t glyphid = e.text.glyph_info[i].codepoint;
			auto gso = f.glyph_positions[glyphid];
			float x_advance = float(gso.x_advance);
			float x_offset = float(e.text.glyph_info[i].x_offset) / 4.f + float(gso.x);
			float y_offset = float(gso.y) - float(e.text.glyph_info[i].y_offset) / 4.f;
			if(glyphid != FT_Get_Char_Index(f.font_face, ' ')) {
				// Add up baseline and kerning offsets
				glm::vec2 glyph_positions{ x_offset / 64.f, -y_offset / 64.f };
				auto dpoly_fn = [&](float x) {
					// y = a + 1bx^1 + 1cx^2 + 1dx^3
					// y = 0 + 1bx^0 + 2cx^1 + 3dx^2
					return e.coeff[1] + 2.f * e.coeff[2] * x + 3.f * e.coeff[3] * x * x;
				};
				glm::vec2 curr_dir = glm::normalize(glm::vec2(effective_ratio, dpoly_fn(x)));
				glm::vec2 curr_normal_dir = glm::vec2(-curr_dir.y, curr_dir.x);
				curr_dir.x *= 0.5f;
				curr_normal_dir.x *= 0.5f;

				glm::vec2 shader_direction = glm::normalize(glm::vec2(e.ratio.x, dpoly_fn(x) * e.ratio.y));

				auto p0 = glm::vec2(x, poly_fn(x)) * e.ratio + e.basis;
				p0 /= glm::vec2(size_x, size_y); // Rescale the coordinate to 0-1
				p0 -= (1.5f - 2.f * glyph_positions.y) * curr_normal_dir * real_text_size;
				p0 += (1.0f + 2.f * glyph_positions.x) * curr_dir * real_text_size;

				float type = float((gso.texture_slot >> 6) % text::max_texture_layers);
				float step = 1.f / 8.f;
				float tx = float(gso.texture_slot & 7) * step;
				float ty = float((gso.texture_slot & 63) >> 3) * step;

				province_text_line_vertices.emplace_back(p0, glm::vec2(-1, 1), shader_direction, glm::vec3(tx, ty, type), real_text_size);
				province_text_line_vertices.emplace_back(p0, glm::vec2(-1, -1), shader_direction, glm::vec3(tx, ty + step, type), real_text_size);
				province_text_line_vertices.emplace_back(p0, glm::vec2(1, -1), shader_direction, glm::vec3(tx + step, ty + step, type), real_text_size);

				province_text_line_vertices.emplace_back(p0, glm::vec2(1, -1), shader_direction, glm::vec3(tx + step, ty + step, type), real_text_size);
				province_text_line_vertices.emplace_back(p0, glm::vec2(1, 1), shader_direction, glm::vec3(tx + step, ty, type), real_text_size);
				province_text_line_vertices.emplace_back(p0, glm::vec2(-1, 1), shader_direction, glm::vec3(tx, ty, type), real_text_size);
			}
			float glyph_advance = x_advance * size / 64.f;
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
	if(province_text_line_vertices.size() > 0) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo_array[vo_province_text_line]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(text_line_vertex) * province_text_line_vertices.size(), &province_text_line_vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

GLuint load_dds_texture(simple_fs::directory const& dir, native_string_view file_name, int soil_flags = ogl::SOIL_FLAG_TEXTURE_REPEATS) {
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
	return ogl::SOIL_direct_load_DDS_from_memory(data, content.file_size, size_x, size_y, soil_flags);
}

emfx::xac_pp_actor_material_layer get_diffuse_layer(emfx::xac_pp_actor_material const& mat) {
	for(const auto& layer : mat.layers) {
		if(layer.map_type == emfx::xac_pp_material_map_type::diffuse) {
			return layer;
		}
	}
	for(const auto& layer : mat.layers) {
		if(strstr(layer.texture.c_str(), "spec") == NULL) {
			return layer;
		}
	}
	return mat.layers.empty()
		? emfx::xac_pp_actor_material_layer{}
		: mat.layers[0];
}

void load_static_meshes(sys::state& state) {
#if 0
	struct static_mesh_vertex {
		glm::vec3 position_;
		glm::vec2 normal_;
		glm::vec2 texture_coord_;
	};
	std::vector<static_mesh_vertex> static_mesh_vertices;
	static const std::array<native_string_view, display_data::max_static_meshes> xac_model_names = {
		NATIVE("capital_bigben"), //0
		NATIVE("capital_eiffeltower"), //1
		NATIVE("Panama_Canel"), //2
		NATIVE("Kiel_Canal"), //3
		NATIVE("Suez_Canal"), //4
		NATIVE("trainstation"), //5 -- railroad present
		NATIVE("Navalbase_Late_Empty"), //6 -- naval base with no ships
		NATIVE("Navalbase_Late_Full"), //7 -- naval base with a docked ship
		NATIVE("Fort_Early"), //8 -- fort
		NATIVE("factory"), //9 -- factory
		NATIVE("Blockade"), //10 -- blockade
		NATIVE("generic_euro_infantry"), //11 -- infantry
		NATIVE("Generic_Frigate"), //12 -- frigate
		NATIVE("Generic_Manowar"), //13 -- manowar
		NATIVE("Generic_Transport_Ship"), //14 -- transport ship
		NATIVE("Horse"), // 15 -- horse
		NATIVE("wake"), // 16 -- ship wake
		NATIVE("Infantry_shadowblob"), // 17 -- shadow blob
		NATIVE("Generic_Euro_Infantry2"), // 18 -- artillery
		NATIVE("buildings_01_1"), // 19 -- housing
		NATIVE("buildings_01_2"), // 20
		NATIVE("buildings_01_3"), // 21
		NATIVE("buildings_02_1"), // 22
		NATIVE("buildings_02_2"), // 23
		NATIVE("buildings_02_3"), // 24
		NATIVE("buildings_03_1"), // 25
		NATIVE("buildings_03_2"), // 26
		NATIVE("buildings_03_3"), // 27
		NATIVE("Fort_Mid2"), //28 -- fort
		NATIVE("Fort_Late"), //29 -- fort
		NATIVE("Navalbase_Early_Empty"), //30 -- naval base with no ships
		NATIVE("Navalbase_Early_Full"), //31 -- naval base with a docked ship
		NATIVE("Navalbase_Mid_Empty"), //32 -- naval base with no ships
		NATIVE("Navalbase_Mid_Full"), //33 -- naval base with a docked ship
		NATIVE("Generic_Battleship"), //34 -- battleship
		NATIVE("Generic_Cruiser"), //35 -- cruiser
		NATIVE("Generic_Ironclad"), //36 -- ironclad
		NATIVE("Generic_Raider"), //37 -- raider
		NATIVE("floating_flag"), //38 -- floating flag
		NATIVE("flag"), //39 -- flag
	};
	static const std::array<float, display_data::max_static_meshes> scaling_factor = {
		1.f, //0
		1.f, //1
		1.f, //2
		1.f, //3
		1.f, //4
		1.f, //5
		1.f, //6
		1.f, //7
		1.f, //8
		0.75f, //9
		1.5f, //10
		1.4f, //11
		2.4f, //12
		0.8f, //13
		1.5f, //14
		1.5f, //15
		1.f, //16
		1.f, //17
		0.7f, //18 -- housing
		0.7f, //19
		0.7f, //20
		0.68f, //21
		0.68f, //22
		0.68f, //23
		0.66f, //24
		0.66f, //25
		0.66f, //26
		1.0f, //27
		1.0f, //28
		1.0f, //29
		1.0f, //30
		1.0f, //31
		1.0f, //32
		1.0f, //33
		1.0f, //34
		1.0f, //35
		1.0f, //36
		1.0f, //37
		1.0f, //38
		1.0f, //39
		1.0f, //40
		1.0f, //41
	};
	constexpr float no_elim = 9999.f + 0.1f;
	constexpr float quad_elim = 9999.f + 0.2f;
	static const std::array<float, display_data::max_static_meshes> elim_factor = {
		-0.1f, //0
		no_elim, //1
		no_elim, //2
		no_elim, //3
		-0.1f, //4
		quad_elim, //5
		-0.1f, //6
		-0.1f, //7
		-0.1f, //8
		quad_elim, //9
		-0.1f, //10
		-0.1f, //11
		-0.1f, //12
		-0.1f, //13
		-0.1f, //14
		no_elim, //15
		-0.1f, //16
		-0.1f, //17
		-0.1f, //18 -- housing
		-0.1f, //19
		-0.1f, //20
		-0.1f, //21
		-0.1f, //22
		-0.1f, //23
		-0.1f, //24
		-0.1f, //25
		-0.1f, //26
		-0.1f, //27
		-0.1f, //28
		-0.1f, //29
		-0.1f, //30
		-0.1f, //31
		-0.1f, //32
		-0.1f, //33
		-0.1f, //34
		-0.1f, //35
		no_elim, //36
		-0.1f, //37
		-0.1f, //38
		-0.1f, //39
		-0.1f, //40
		-0.1f, //41
	};
	auto root = simple_fs::get_root(state.common_fs);
	auto gfx_anims = simple_fs::open_directory(root, NATIVE("gfx/anims"));

	state.map_state.map_data.static_mesh_counts.resize(display_data::max_static_meshes);
	state.map_state.map_data.static_mesh_starts.resize(display_data::max_static_meshes);
	for(uint32_t k = 0; k < display_data::max_static_meshes; k++) {
		auto old_size = static_mesh_vertices.size();
		auto f = simple_fs::open_file(gfx_anims, native_string(xac_model_names[k]) + NATIVE(".xac"));
		if(f) {
			parsers::error_handler err(simple_fs::native_to_utf8(simple_fs::get_full_name(*f)));
			auto contents = simple_fs::view_contents(*f);
			emfx::xac_context context{};
			emfx::parse_xac(context, contents.data, contents.data + contents.file_size, err);
			emfx::finish(context);

			auto& texid = state.map_state.map_data.static_mesh_textures[k];
			for(auto const& node : context.nodes) {
				int32_t mesh_index = 0;
				for(auto const& mesh : node.meshes) {
					bool is_collision = node.collision_mesh == mesh_index;
					bool is_visual = node.visual_mesh == mesh_index;

					uint32_t vertex_offset = 0;
					for(auto const& sub : mesh.submeshes) {
						for(uint32_t i = 0; i < uint32_t(sub.indices.size()); i += 3) {
							static_mesh_vertex triangle_vertices[3];
							for(uint32_t j = 0; j < 3; j++) {
								static_mesh_vertex smv;
								auto index = sub.indices[i + j] + vertex_offset;
								auto vv = mesh.vertices[index % mesh.vertices.size()];
								auto vn = mesh.normals.empty()
									? emfx::xac_vector3f{ vv.x, vv.y, vv.z }
								: mesh.normals[index % mesh.normals.size()];
								auto vt = mesh.texcoords.empty()
									? emfx::xac_vector2f{ vv.x, vv.y }
								: mesh.texcoords[index % mesh.texcoords.size()];
								smv.position_ = glm::vec3(vv.x, vv.y, vv.z);
								smv.normal_ = glm::vec3(vn.x, vn.y, vn.z);
								smv.texture_coord_ = glm::vec2(vt.x, vt.y);
								triangle_vertices[j] = smv;
							}
							// Clip standing planes (some models have flat planes
							// beneath them)
							bool keep = is_visual;
							if(elim_factor[k] == no_elim) {
								if(is_visual) {
									for(const auto& smv : triangle_vertices) {
										static_mesh_vertex tmp = smv;
										tmp.position_ *= scaling_factor[k];
										static_mesh_vertices.push_back(tmp);
									}
								}
							} else if(elim_factor[k] == quad_elim) {
								if(triangle_vertices[0].position_.y <= -0.1f
									|| triangle_vertices[1].position_.y <= -0.1f
									|| triangle_vertices[2].position_.y <= -0.1f) {
									for(const auto& smv : triangle_vertices) {
										static_mesh_vertex tmp = smv;
										tmp.position_ *= scaling_factor[k];
										static_mesh_vertices.push_back(tmp);
									}
								}
							} else {
								if(triangle_vertices[0].position_.y <= elim_factor[k]
									&& triangle_vertices[1].position_.y <= elim_factor[k]
									&& triangle_vertices[2].position_.y <= elim_factor[k]) {
									for(const auto& smv : triangle_vertices) {
										static_mesh_vertex tmp = smv;
										tmp.position_ *= scaling_factor[k];
										static_mesh_vertices.push_back(tmp);
									}
								}
							}
						}
						vertex_offset += sub.num_vertices;
						// This is how most models fallback to find their textures...
						if(!texid) {
							auto const& mat = context.materials[sub.material_id];
							auto const& layer = get_diffuse_layer(mat);
							if(layer.texture.empty()) {
								texid = load_dds_texture(gfx_anims, native_string(xac_model_names[k]) + NATIVE("Diffuse.dds"), 0);
								if(!texid) {
									texid = load_dds_texture(gfx_anims, native_string(xac_model_names[k]) + NATIVE("_Diffuse.dds"), 0);
									if(!texid) {
										texid = load_dds_texture(gfx_anims, native_string(xac_model_names[k]) + NATIVE(".dds"), 0);
									}
								}
							} else {
								texid = load_dds_texture(gfx_anims, simple_fs::utf8_to_native(layer.texture + "Diffuse.dds"), 0);
								if(!texid) {
									texid = load_dds_texture(gfx_anims, simple_fs::utf8_to_native(layer.texture + "_Diffuse.dds"), 0);
									if(!texid) {
										texid = load_dds_texture(gfx_anims, simple_fs::utf8_to_native(layer.texture + ".dds"), 0);
									}
								}
							}
						}
					}
					mesh_index++;
				}
			}

			state.map_state.map_data.static_mesh_starts[k] = GLint(old_size);
			state.map_state.map_data.static_mesh_counts[k] = GLsizei(static_mesh_vertices.size() - old_size);
		} else {
			state.map_state.map_data.static_mesh_starts[k] = GLint(old_size);
			state.map_state.map_data.static_mesh_counts[k] = GLsizei(0);
		}
	}

	if(!static_mesh_vertices.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, state.map_state.map_data.vbo_array[state.map_state.map_data.vo_static_mesh]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(static_mesh_vertex) * static_mesh_vertices.size(), &static_mesh_vertices[0], GL_STATIC_DRAW);
	}
	glBindVertexArray(state.map_state.map_data.vao_array[state.map_state.map_data.vo_static_mesh]);
	glBindVertexBuffer(0, state.map_state.map_data.vbo_array[state.map_state.map_data.vo_static_mesh], 0, sizeof(static_mesh_vertex)); // Bind the VBO to 0 of the VAO
	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, offsetof(static_mesh_vertex, position_)); // Set up vertex attribute format for the position
	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, offsetof(static_mesh_vertex, normal_)); // Set up vertex attribute format for the normal direction
	glVertexAttribFormat(2, 2, GL_FLOAT, GL_FALSE, offsetof(static_mesh_vertex, texture_coord_)); // Set up vertex attribute format for the texture coordinates
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribBinding(0, 0);
	glVertexAttribBinding(1, 0);
	glVertexAttribBinding(2, 0);
	glBindVertexArray(0);
#endif
}

void display_data::load_map(sys::state& state) {
	auto root = simple_fs::get_root(state.common_fs);
	glGenVertexArrays(vo_count, vao_array);
	glGenBuffers(vo_count, vbo_array);
	load_shaders(root);
	load_static_meshes(state);
	create_meshes();

	state.console_log(ogl::opengl_get_error_name(glGetError()));

	

	auto assets_dir = simple_fs::open_directory(root, NATIVE("assets"));
	auto map_dir = simple_fs::open_directory(root, NATIVE("map"));
	auto map_terrain_dir = simple_fs::open_directory(map_dir, NATIVE("terrain"));
	auto map_items_dir = simple_fs::open_directory(root, NATIVE("gfx/mapitems"));
	auto gfx_anims_dir = simple_fs::open_directory(root, NATIVE("gfx/anims"));

	glGenTextures(1, &textures[texture_diag_border_identifier]);
	if(textures[texture_diag_border_identifier]) {
		assert(!diagonal_borders.empty());

		glBindTexture(GL_TEXTURE_2D, textures[texture_diag_border_identifier]);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8UI, size_x, size_y);
		assert(size_x > 0);
		assert(size_y > 0);
		// assert(diagonal_borders.size() != size_x * size_y);
		assert(size_x % 4 == 0); // The texture data might have incorrect row alignment (default is 4 bytes). If the texture width is not a multiple of 4, this can corrupt the upload.
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size_x, size_y, GL_RED_INTEGER, GL_UNSIGNED_BYTE, diagonal_borders.data());
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	ogl::set_gltex_parameters(textures[texture_diag_border_identifier], GL_TEXTURE_2D, GL_NEAREST, GL_CLAMP_TO_EDGE);
	

	textures[texture_terrain] = ogl::make_gl_texture(&terrain_id_map[0], size_x, size_y, 1);
	ogl::set_gltex_parameters(textures[texture_terrain], GL_TEXTURE_2D, GL_NEAREST, GL_CLAMP_TO_EDGE);

	textures[texture_provinces] = load_province_map(province_id_map, size_x, size_y);
	auto texturesheet = open_file(map_terrain_dir, NATIVE("texturesheet.png"));
	if(!texturesheet) {
		texturesheet = open_file(map_terrain_dir, NATIVE("texturesheet.tga"));
		if(!texturesheet) {
			texturesheet = open_file(map_terrain_dir, NATIVE("texturesheet.dds"));
		}
	}
	texture_arrays[texture_array_terrainsheet] = ogl::load_texture_array_from_file(*texturesheet, 8, 8);

	textures[texture_water_normal] = load_dds_texture(map_terrain_dir, NATIVE("sea_normal.dds"));
	if(!textures[texture_water_normal]) textures[texture_water_normal] = ogl::make_gl_texture(map_items_dir, NATIVE("sea_normal.png"));
	
	textures[texture_colormap_water] = load_dds_texture(map_terrain_dir, NATIVE("colormap_water.dds"));
	if(!textures[texture_colormap_water]) textures[texture_colormap_water] = ogl::make_gl_texture(map_items_dir, NATIVE("colormap_water.png"));
	
	textures[texture_colormap_terrain] = load_dds_texture(map_terrain_dir, NATIVE("colormap.dds"));
	if(!textures[texture_colormap_terrain]) textures[texture_colormap_terrain] = ogl::make_gl_texture(map_items_dir, NATIVE("colormap.png"));

	textures[texture_colormap_political] = load_dds_texture(map_terrain_dir, NATIVE("colormap_political.dds"));
	if(!textures[texture_colormap_political]) textures[texture_colormap_political] = ogl::make_gl_texture(map_items_dir, NATIVE("colormap_political.png"));
	
	textures[texture_overlay] = load_dds_texture(map_terrain_dir, NATIVE("map_overlay_tile.dds"));
	if(!textures[texture_overlay]) textures[texture_overlay] = ogl::make_gl_texture(map_items_dir, NATIVE("map_overlay_tile.png"));
	
	textures[texture_stripes] = load_dds_texture(map_terrain_dir, NATIVE("stripes.dds"));
	if(!textures[texture_stripes]) textures[texture_stripes] = ogl::make_gl_texture(map_items_dir, NATIVE("stripes.png"));
	
	textures[texture_river_body] = load_dds_texture(assets_dir, NATIVE("river.dds"));
	ogl::set_gltex_parameters(textures[texture_river_body], GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);

	textures[texture_national_border] = load_dds_texture(assets_dir, NATIVE("nat_border.dds"));
	ogl::set_gltex_parameters(textures[texture_national_border], GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_CLAMP);

	textures[texture_state_border] = load_dds_texture(assets_dir, NATIVE("state_border.dds"));
	ogl::set_gltex_parameters(textures[texture_state_border], GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_CLAMP);

	textures[texture_prov_border] = load_dds_texture(assets_dir, NATIVE("prov_border.dds"));
	ogl::set_gltex_parameters(textures[texture_prov_border], GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_CLAMP);

	textures[texture_imp_border] = load_dds_texture(assets_dir, NATIVE("imp_border.dds"));
	ogl::set_gltex_parameters(textures[texture_imp_border], GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_CLAMP);

	textures[texture_coastal_border] = load_dds_texture(assets_dir, NATIVE("coastborder.dds"));
	ogl::set_gltex_parameters(textures[texture_coastal_border], GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, GL_CLAMP);

	textures[texture_railroad] = load_dds_texture(gfx_anims_dir, NATIVE("railroad.dds"));
	ogl::set_gltex_parameters(textures[texture_railroad], GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);

	textures[texture_unit_arrow] = ogl::make_gl_texture(map_items_dir, NATIVE("movearrow.tga"));
	ogl::set_gltex_parameters(textures[texture_unit_arrow], GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_CLAMP_TO_EDGE);

	textures[texture_arrow] = ogl::make_gl_texture(assets_dir, NATIVE("arrow.png"));
	ogl::set_gltex_parameters(textures[texture_arrow], GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);

	textures[texture_attack_unit_arrow] = ogl::make_gl_texture(map_items_dir, NATIVE("attackarrow.tga"));
	ogl::set_gltex_parameters(textures[texture_attack_unit_arrow], GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_CLAMP_TO_EDGE);

	textures[texture_retreat_unit_arrow] = ogl::make_gl_texture(map_items_dir, NATIVE("retreatarrow.tga"));
	ogl::set_gltex_parameters(textures[texture_retreat_unit_arrow], GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_CLAMP_TO_EDGE);

	textures[texture_strategy_unit_arrow] = ogl::make_gl_texture(map_items_dir, NATIVE("stratarrow.tga"));
	ogl::set_gltex_parameters(textures[texture_strategy_unit_arrow], GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_CLAMP_TO_EDGE);

	textures[texture_objective_unit_arrow] = ogl::make_gl_texture(map_items_dir, NATIVE("objectivearrow.tga"));
	ogl::set_gltex_parameters(textures[texture_objective_unit_arrow], GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_CLAMP_TO_EDGE);

	textures[texture_other_objective_unit_arrow] = ogl::make_gl_texture(map_items_dir, NATIVE("otherobjectivearrow.tga"));
	ogl::set_gltex_parameters(textures[texture_other_objective_unit_arrow], GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_CLAMP_TO_EDGE);

	textures[texture_hover_border] = load_dds_texture(assets_dir, NATIVE("hover_border.dds"));
	ogl::set_gltex_parameters(textures[texture_imp_border], GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);

	// Get the province_color handle
	// province_color is an array of 2 textures, one for province and the other for stripes
	glGenTextures(1, &texture_arrays[texture_array_province_color]);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture_arrays[texture_array_province_color]);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 256, 256, 2);
	ogl::set_gltex_parameters(texture_arrays[texture_array_province_color], GL_TEXTURE_2D_ARRAY, GL_NEAREST, GL_CLAMP_TO_EDGE);

	// Get the province_highlight handle
	glGenTextures(1, &textures[texture_province_highlight]);
	glBindTexture(GL_TEXTURE_2D, textures[texture_province_highlight]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 256, 256);
	ogl::set_gltex_parameters(textures[texture_province_highlight], GL_TEXTURE_2D, GL_NEAREST, GL_CLAMP_TO_EDGE);

	// Get the province_fow handle
	glGenTextures(1, &textures[texture_province_fow]);
	glBindTexture(GL_TEXTURE_2D, textures[texture_province_fow]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 256, 256);
	ogl::set_gltex_parameters(textures[texture_province_fow], GL_TEXTURE_2D, GL_NEAREST, GL_CLAMP_TO_EDGE);

	// set up sea texture:
	glGenTextures(1, &textures[texture_sea_mask]);
	glBindTexture(GL_TEXTURE_2D, textures[texture_sea_mask]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 256, 256);
	ogl::set_gltex_parameters(textures[texture_sea_mask], GL_TEXTURE_2D, GL_NEAREST, GL_CLAMP_TO_EDGE);
	province_id_sea_mask.resize(state.world.province_size() + 1);
	for(auto p : state.world.in_province) {
		if(p.id.index() >= state.province_definitions.first_sea_province.index()) {
			province_id_sea_mask[province::to_map_id(p)] = 0xFFFFFFFF;
		} else {
			province_id_sea_mask[province::to_map_id(p)] = 0x00000000;
		}
	}
	gen_prov_color_texture(textures[texture_sea_mask], province_id_sea_mask);


	glBindTexture(GL_TEXTURE_2D, 0);

	uint32_t province_size = state.world.province_size() + 1;
	province_size += 256 - province_size % 256;

	std::vector<uint32_t> test_highlight(province_size);
	gen_prov_color_texture(textures[texture_province_highlight], test_highlight);
	for(uint32_t i = 0; i < test_highlight.size(); ++i) {
		test_highlight[i] = 255;
	}
	std::vector<uint32_t> test_color(province_size * 4);
	for(uint32_t i = 0; i < test_color.size(); ++i) {
		test_color[i] = 255;
	}
	set_province_color(test_color);
}

} // namespace map
