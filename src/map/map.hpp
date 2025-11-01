#pragma once

#include <cstdint>
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include "glew.h"

#include "map_modes.hpp"
#include "opengl_wrapper.hpp"
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

namespace sys {
struct state;
};
namespace parsers {
struct scenario_building_context;
};

namespace map {

struct map_vertex {
	map_vertex(float x, float y) : position_(x, y){};
	glm::vec2 position_;
};

struct vertex {
	glm::vec2 position_;
	glm::vec2 texcoord_;
};

struct trade_particle {
	glm::vec2 position_;
	glm::vec2 target_;
	int trade_graph_node_current;
	int trade_graph_node_prev = -1;
	int trade_graph_node_next;
};
struct screen_vertex {
	screen_vertex(float x, float y) : position_(x, y){};
	glm::vec2 position_;
};

struct curved_line_vertex {
	curved_line_vertex(){};
	curved_line_vertex(glm::vec2 position, glm::vec2 normal_direction, glm::vec2 direction, glm::vec2 texture_coord, float type)
			: position_(position), normal_direction_(normal_direction), direction_(direction), texture_coord_(texture_coord), type_{type} {};
	glm::vec2 position_;
	glm::vec2 normal_direction_;
	glm::vec2 direction_;
	glm::vec2 texture_coord_;
	float type_ = 0.f;
};

struct textured_line_vertex {
	glm::vec2 position_;
	glm::vec2 normal_direction_;
	float texture_coordinate_ = 0.f;
	float distance_ = 0.f;
};

struct textured_line_with_width_vertex {
	glm::vec2 position_;
	glm::vec2 normal_direction_;
	float texture_coordinate_ = 0.f;
	float distance_ = 0.f;
	float width_ = 0.f;
};

struct textured_line_vertex_b {
	glm::vec2 position;
	glm::vec2 previous_point;
	glm::vec2 next_point;
	float texture_coordinate = 0.f;
	float distance = 0.f;
};

struct textured_line_vertex_b_enriched_with_province_index {
	glm::vec2 position;
	glm::vec2 previous_point;
	glm::vec2 next_point;
	uint16_t province_index;
	float texture_coordinate = 0.f;
	float distance = 0.f;
};

struct text_line_vertex {
	text_line_vertex() { };
	text_line_vertex(glm::vec2 position, glm::vec2 normal_direction, glm::vec2 direction, glm::vec3 texture_coord, float thickness)
		: position_(position), normal_direction_(normal_direction), direction_(direction), texture_coord_(texture_coord), thickness_{ thickness }  { };
	glm::vec2 position_;
	glm::vec2 normal_direction_;
	glm::vec2 direction_;
	glm::vec3 texture_coord_;
	float thickness_ = 0.f;
};

struct text_line_generator_data {
	text_line_generator_data() { };
	text_line_generator_data(text::stored_glyphs&& text_, glm::vec4 coeff_, glm::vec2 basis_, glm::vec2 ratio_) : text(std::move(text_)), coeff{ coeff_ }, basis{ basis_ }, ratio{ ratio_ } { };
	text::stored_glyphs text;
	glm::vec4 coeff{0.f};
	glm::vec2 basis{0.f};
	glm::vec2 ratio{0.f};
};

struct border {
	int start_index = 0;
	int count = 0;
	dcon::province_adjacency_id adj;
	uint16_t padding = 0;
};

enum class map_view;
class display_data {
public:
	display_data(){};
	~display_data();

	// Called to load the terrain and province map data
	void load_map_data(parsers::scenario_building_context& context);
	// Called to load the map. Will load the texture and shaders from disk
	void load_map(sys::state& state);

	bool texturesheet_is_dds = false;

	void render(sys::state& state, glm::vec2 screen_size, glm::vec2 offset, float zoom, map_view map_view_mode, map_mode::mode active_map_mode,
			glm::mat3 globe_rotation, float time_counter);
	void update_borders(sys::state& state);
	void update_fog_of_war(sys::state& state);
	void update_highlight(sys::state& state);
	void set_province_color(std::vector<uint32_t> const& prov_color);
	void set_drag_box(bool draw_box, glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pixel_size);
	void update_sprawl(sys::state& state);
	void set_text_lines(sys::state& state, std::vector<text_line_generator_data> const& data);
	void set_province_text_lines(sys::state& state, std::vector<text_line_generator_data> const& data);

	std::vector<border> borders;
	std::vector<textured_line_vertex_b_enriched_with_province_index> border_vertices;
	//
	std::vector<textured_line_with_width_vertex> river_vertices;
	std::vector<GLint> river_starts;
	std::vector<GLsizei> river_counts;
	// ROADS
	std::vector<textured_line_vertex> railroad_vertices;
	std::vector<GLint> railroad_starts;
	std::vector<GLsizei> railroad_counts;
	// CITY VERTICES
	std::vector<vertex> city_vertices;
	std::vector<GLint> city_starts;
	std::vector<GLsizei> city_counts;
	//
	std::vector<textured_line_vertex_b> coastal_vertices;
	std::vector<GLint> coastal_starts;
	std::vector<GLsizei> coastal_counts;
	//
	std::vector<GLint> static_mesh_starts;
	std::vector<GLsizei> static_mesh_counts;
	//
	std::vector<textured_line_with_width_vertex> trade_flow_vertices;
	std::vector<GLint> trade_flow_arrow_starts;
	std::vector<GLsizei> trade_flow_arrow_counts;
	// trade particles
	std::vector<trade_particle> trade_particles_positions;
	ankerl::unordered_dense::map<int, ankerl::unordered_dense::map<int, float>> particle_next_node_probability;
	ankerl::unordered_dense::map<int, float> particle_creation_probability;
	ankerl::unordered_dense::map<int, glm::vec2> trade_node_position;
	//
	std::vector<curved_line_vertex> unit_arrow_vertices;
	std::vector<GLint> unit_arrow_starts;
	std::vector<GLsizei> unit_arrow_counts;
	//
	std::vector<curved_line_vertex> attack_unit_arrow_vertices;
	std::vector<GLint> attack_unit_arrow_starts;
	std::vector<GLsizei> attack_unit_arrow_counts;
	//
	std::vector<curved_line_vertex> retreat_unit_arrow_vertices;
	std::vector<GLint> retreat_unit_arrow_starts;
	std::vector<GLsizei> retreat_unit_arrow_counts;
	//
	std::vector<curved_line_vertex> strategy_unit_arrow_vertices;
	std::vector<GLint> strategy_unit_arrow_starts;
	std::vector<GLsizei> strategy_unit_arrow_counts;
	//
	std::vector<curved_line_vertex> objective_unit_arrow_vertices;
	std::vector<GLint> objective_unit_arrow_starts;
	std::vector<GLsizei> objective_unit_arrow_counts;
	//
	std::vector<curved_line_vertex> other_objective_unit_arrow_vertices;
	std::vector<GLint> other_objective_unit_arrow_starts;
	std::vector<GLsizei> other_objective_unit_arrow_counts;
	//
	std::vector<GLuint> text_line_texture_per_quad;
	std::vector<text_line_vertex> text_line_vertices;
	std::vector<text_line_vertex> province_text_line_vertices;
	std::vector<screen_vertex> drag_box_vertices;
	std::vector<uint8_t> terrain_id_map;
	std::vector<uint8_t> median_terrain_type;
	std::vector<uint32_t> province_area;
	std::vector<float> province_area_km2;
	std::vector<uint8_t> diagonal_borders;

	// map pixel -> province id
	std::vector<uint16_t> province_id_map;
	std::vector<uint16_t> map_indices;

	// province id mask to detect seas
	std::vector<uint32_t> province_id_sea_mask;

	uint32_t size_x;
	uint32_t size_y;
	uint32_t land_vertex_count = 0;
	float world_circumference;

	// Meshes
	static constexpr uint32_t vo_land = 0;
	static constexpr uint32_t vo_border = 1;
	static constexpr uint32_t vo_river = 2;
	static constexpr uint32_t vo_unit_arrow = 3;
	static constexpr uint32_t vo_text_line = 4;
	static constexpr uint32_t vo_drag_box = 5;
	static constexpr uint32_t vo_coastal = 6;
	static constexpr uint32_t vo_railroad = 7;
	static constexpr uint32_t vo_static_mesh = 8;
	static constexpr uint32_t vo_province_text_line = 9;
	static constexpr uint32_t vo_attack_unit_arrow = 10;
	static constexpr uint32_t vo_retreat_unit_arrow = 11;
	static constexpr uint32_t vo_strategy_unit_arrow = 12;
	static constexpr uint32_t vo_objective_unit_arrow = 13;
	static constexpr uint32_t vo_other_objective_unit_arrow = 14;
	static constexpr uint32_t vo_trade_flow = 15;
	static constexpr uint32_t vo_square = 16;
	static constexpr uint32_t vo_cities = 17;
	static constexpr uint32_t vo_count = 18;
	GLuint vao_array[vo_count] = { 0 };
	GLuint vbo_array[vo_count] = { 0 };
	// Textures
	static constexpr uint32_t texture_provinces = 0;
	static constexpr uint32_t texture_terrain = 1;
	static constexpr uint32_t texture_water_normal = 2;
	static constexpr uint32_t texture_colormap_water = 3;
	static constexpr uint32_t texture_colormap_terrain = 4;
	static constexpr uint32_t texture_colormap_political = 5;
	static constexpr uint32_t texture_overlay = 6;
	static constexpr uint32_t texture_province_highlight = 7;
	static constexpr uint32_t texture_stripes = 8;
	static constexpr uint32_t texture_river_body = 9;
	static constexpr uint32_t texture_national_border = 10;
	static constexpr uint32_t texture_state_border = 11;
	static constexpr uint32_t texture_prov_border = 12;
	static constexpr uint32_t texture_imp_border = 13;
	static constexpr uint32_t texture_unit_arrow = 14;
	static constexpr uint32_t texture_province_fow = 15;
	static constexpr uint32_t texture_coastal_border = 16;
	static constexpr uint32_t texture_diag_border_identifier = 17;
	static constexpr uint32_t texture_railroad = 18;
	static constexpr uint32_t texture_attack_unit_arrow = 19;
	static constexpr uint32_t texture_retreat_unit_arrow = 20;
	static constexpr uint32_t texture_strategy_unit_arrow = 21;
	static constexpr uint32_t texture_objective_unit_arrow = 22;
	static constexpr uint32_t texture_other_objective_unit_arrow = 23;
	static constexpr uint32_t texture_hover_border = 24;
	static constexpr uint32_t texture_sea_mask = 25;
	static constexpr uint32_t texture_arrow = 26;
	static constexpr uint32_t texture_city = 27;
	static constexpr uint32_t texture_count = 28;
	GLuint textures[texture_count] = { 0 };
	// Texture Array
	static constexpr uint32_t texture_array_terrainsheet = 0;
	static constexpr uint32_t texture_array_province_color = 1;
	static constexpr uint32_t texture_array_count = 2;
	GLuint texture_arrays[texture_array_count] = { 0 };
	// Shaders
	static constexpr uint32_t shader_terrain = 0;
	static constexpr uint32_t shader_line_border = 1;
	static constexpr uint32_t shader_textured_line = 2;
	static constexpr uint32_t shader_line_unit_arrow = 3;
	static constexpr uint32_t shader_text_line = 4;
	static constexpr uint32_t shader_drag_box = 5;
	static constexpr uint32_t shader_borders = 6;
	static constexpr uint32_t shader_railroad_line = 7;
	static constexpr uint32_t shader_map_standing_object = 8;
	static constexpr uint32_t shader_textured_line_with_variable_width = 9;
	static constexpr uint32_t shader_trade_flow = 10;
	static constexpr uint32_t shader_provinces = 11;
	static constexpr uint32_t shader_borders_provinces = 12;
	static constexpr uint32_t shader_map_sprite = 13;
	static constexpr uint32_t shader_textured_triangle = 14;
	static constexpr uint32_t shader_count = 15;
	GLuint shaders[shader_count] = { 0 };

	static constexpr uint32_t uniform_offset = 0;
	static constexpr uint32_t uniform_aspect_ratio = 1;
	static constexpr uint32_t uniform_zoom = 2;
	static constexpr uint32_t uniform_map_size = 3;
	static constexpr uint32_t uniform_rotation = 4;
	static constexpr uint32_t uniform_gamma = 5;
	static constexpr uint32_t uniform_subroutines_index = 6;
	static constexpr uint32_t uniform_time = 7;
	//
	static constexpr uint32_t uniform_provinces_texture_sampler = 8;
	static constexpr uint32_t uniform_terrain_texture_sampler = 9;
	static constexpr uint32_t uniform_terrainsheet_texture_sampler = 10;
	static constexpr uint32_t uniform_water_normal = 11;
	static constexpr uint32_t uniform_colormap_water = 12;
	static constexpr uint32_t uniform_colormap_terrain = 13;
	static constexpr uint32_t uniform_overlay = 14;
	static constexpr uint32_t uniform_province_color = 15;
	static constexpr uint32_t uniform_colormap_political = 16;
	static constexpr uint32_t uniform_province_highlight = 17;
	static constexpr uint32_t uniform_stripes_texture = 18;
	static constexpr uint32_t uniform_province_fow = 19;
	static constexpr uint32_t uniform_diag_border_identifier = 20;
	static constexpr uint32_t uniform_width = 21;
	static constexpr uint32_t uniform_subroutines_index_2 = 22;
	static constexpr uint32_t uniform_line_texture = 23;
	static constexpr uint32_t uniform_texture_sampler = 24;
	static constexpr uint32_t uniform_opaque = 25;
	static constexpr uint32_t uniform_is_black = 26;
	static constexpr uint32_t uniform_border_width = 27;
	static constexpr uint32_t uniform_unit_arrow = 28;
	static constexpr uint32_t uniform_model_offset = 29;
	static constexpr uint32_t uniform_target_facing = 30;
	static constexpr uint32_t uniform_target_topview_fixup = 31;
	static constexpr uint32_t uniform_provinces_sea_mask = 32;
	static constexpr uint32_t uniform_provinces_real_texture_sampler = 33;
	static constexpr uint32_t uniform_screen_size = 34;
	static constexpr uint32_t uniform_sprite_offsets = 35;
	static constexpr uint32_t uniform_sprite_scale = 36;
	static constexpr uint32_t uniform_sprite_texture_start = 37;
	static constexpr uint32_t uniform_sprite_texture_size = 38;
	static constexpr uint32_t uniform_is_national_border = 39;
	static constexpr uint32_t uniform_graphics_mode = 40;
	static constexpr uint32_t uniform_light_direction = 41;
	static constexpr uint32_t uniform_ignore_light = 42;
	static constexpr uint32_t uniform_terrainsheet_texture_sampler_array = 43;
	static constexpr uint32_t uniform_terrain_is_array = 44;
	static constexpr uint32_t uniform_map_mode_is_data = 45;
	static constexpr uint32_t uniform_count = 46;
	GLuint shader_uniforms[shader_count][uniform_count] = { };

	// models: Textures for static meshes
	static constexpr uint32_t max_static_meshes = 42;
	GLuint static_mesh_textures[max_static_meshes] = { 0 };

	void load_border_data(parsers::scenario_building_context& context);
	void create_border_ogl_objects();
	void load_province_data(parsers::scenario_building_context& context, ogl::image& image);
	void load_provinces_mid_point(parsers::scenario_building_context& context);
	void load_terrain_data(parsers::scenario_building_context& context);
	void load_median_terrain_type(parsers::scenario_building_context& context);

	uint16_t safe_get_province(glm::ivec2 pt);
	void make_coastal_borders(sys::state& state, std::vector<bool>& visited);
	void make_borders(sys::state& state, std::vector<bool>& visited);

	void load_shaders(simple_fs::directory& root);
	void update_borders_mesh();
	void create_meshes();
	void gen_prov_color_texture(GLuint texture_handle, std::vector<uint32_t> const& prov_color, uint8_t layers = 1);

	void create_curved_river_vertices(parsers::scenario_building_context& context, std::vector<uint8_t> const& river_data, std::vector<uint8_t> const& terrain_data);
};

void load_river_crossings(parsers::scenario_building_context& context, std::vector<uint8_t> const& river_data, glm::ivec2 map_size);

void make_navy_path(sys::state& state, std::vector<map::curved_line_vertex>& buffer, dcon::navy_id selected_navy, float size_x, float size_y);
void make_navy_direction(sys::state& state, std::vector<map::curved_line_vertex>& buffer, dcon::navy_id selected_navy, float size_x, float size_y);
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
);
void make_land_path(
	sys::state& state,
	std::vector<map::textured_line_with_width_vertex>& buffer,
	dcon::province_id origin,
	dcon::province_id target,
	float width,
	float size_x,
	float size_y
);

void make_army_path(sys::state& state, std::vector<map::curved_line_vertex>& buffer, dcon::army_id selected_army, float size_x, float size_y);
void make_army_direction(sys::state& state, std::vector<map::curved_line_vertex>& buffer, dcon::army_id selected_army, float size_x, float size_y);

glm::vec2 put_in_local(glm::vec2 new_point, glm::vec2 base_point, float size_x);
void add_bezier_to_buffer(std::vector<map::curved_line_vertex>& buffer, glm::vec2 start, glm::vec2 end, glm::vec2 start_per, glm::vec2 end_per, float progress, bool last_curve, float size_x, float size_y, uint32_t num_b_segments);
void add_tl_bezier_to_buffer(std::vector<map::textured_line_vertex>& buffer, glm::vec2 start, glm::vec2 end, glm::vec2 start_per, glm::vec2 end_per, float progress, bool last_curve, float size_x, float size_y, uint32_t num_b_segments, float& distance);
void add_bezier_to_buffer_variable_width(std::vector<map::textured_line_with_width_vertex>& buffer, glm::vec2 start, glm::vec2 end, glm::vec2 start_tangent, glm::vec2 end_tangent, float progress, bool last_curve, float size_x, float size_y, uint32_t num_b_segments, float& distance, float width_start, float width_middle, float width_end);

} // namespace map
