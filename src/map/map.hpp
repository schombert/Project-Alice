#pragma once

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include "glew.h"

#include "map_modes.hpp"
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

namespace sys {
struct state;
};
namespace parsers {
struct scenario_building_context;
};

namespace map {

struct image {
	uint8_t* data = nullptr;
	int32_t size_x = 0;
	int32_t size_y = 0;
	int32_t channels = 0;

	image() { }

	image(uint8_t* data, int32_t size_x, int32_t size_y, int32_t channels) {
		this->data = data;
		this->size_x = size_x;
		this->size_y = size_y;
		this->channels = channels;
	}
	image(image const& other) = delete;

	image(image&& other) noexcept {
		data = other.data;
		size_x = other.size_x;
		size_y = other.size_y;
		channels = other.channels;
		other.data = nullptr;
	}

	image& operator=(image&& other) noexcept {
		data = other.data;
		size_x = other.size_x;
		size_y = other.size_y;
		channels = other.channels;
		other.data = nullptr;
		return *this;
	}
	image& operator=(image const& other) = delete;

	~image() {
		if(data)
			free(data);
	}
};
struct map_vertex {
	map_vertex(float x, float y) : position_(x, y){};
	glm::vec2 position_;
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

struct textured_line_vertex_b {
	glm::vec2 position;
	glm::vec2 previous_point;
	glm::vec2 next_point;
	float texture_coordinate = 0.f;
	float distance = 0.f;
};

struct text_line_vertex {
	text_line_vertex() { };
	text_line_vertex(glm::vec2 position, glm::vec2 normal_direction, glm::vec2 direction, glm::vec2 texture_coord, float type, float thickness)
		: position_(position), normal_direction_(normal_direction), direction_(direction), texture_coord_(texture_coord), type_{ type }, thickness_{ thickness }  { };
	glm::vec2 position_;
	glm::vec2 normal_direction_;
	glm::vec2 direction_;
	glm::vec2 texture_coord_;
	float type_ = 0.f;
	float thickness_ = 0.f;
};

struct text_line_generator_data {
	text_line_generator_data() { };
	text_line_generator_data(std::string text_, glm::vec4 coeff_, glm::vec2 basis_, glm::vec2 ratio_) : text(text_), coeff{ coeff_ }, basis{ basis_ }, ratio{ ratio_ } { };
	std::string text{};
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

	void render(sys::state& state, glm::vec2 screen_size, glm::vec2 offset, float zoom, map_view map_view_mode, map_mode::mode active_map_mode,
			glm::mat3 globe_rotation, float time_counter);
	void update_borders(sys::state& state);
	void update_fog_of_war(sys::state& state);
	void set_selected_province(sys::state& state, dcon::province_id province_id);
	void set_province_color(std::vector<uint32_t> const& prov_color);
	void set_drag_box(bool draw_box, glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pixel_size);
	void update_railroad_paths(sys::state& state);
	void set_text_lines(sys::state& state, std::vector<text_line_generator_data> const& data);

	std::vector<border> borders;
	std::vector<textured_line_vertex_b> border_vertices;
	std::vector<textured_line_vertex> river_vertices;
	std::vector<GLint> river_starts;
	std::vector<GLsizei> river_counts;
	std::vector<textured_line_vertex> railroad_vertices;
	std::vector<GLint> railroad_starts;
	std::vector<GLsizei> railroad_counts;
	std::vector<textured_line_vertex_b> coastal_vertices;
	std::vector<GLint> coastal_starts;
	std::vector<GLsizei> coastal_counts;
	std::vector<GLint> static_mesh_starts;
	std::vector<GLsizei> static_mesh_counts;
	std::vector<curved_line_vertex> unit_arrow_vertices;
	std::vector<GLint> unit_arrow_starts;
	std::vector<GLsizei> unit_arrow_counts;
	std::vector<text_line_vertex> text_line_vertices;
	std::vector<screen_vertex> drag_box_vertices;
	std::vector<uint8_t> terrain_id_map;
	std::vector<uint8_t> median_terrain_type;
	std::vector<uint32_t> province_area;
	std::vector<uint8_t> diagonal_borders;

	// map pixel -> province id
	std::vector<uint16_t> province_id_map;
	std::vector<uint16_t> map_indices;

	uint32_t size_x;
	uint32_t size_y;
	uint32_t land_vertex_count = 0;

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
	static constexpr uint32_t vo_count = 9;
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
	static constexpr uint32_t texture_count = 19;
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
	static constexpr uint32_t shader_count = 9;
	GLuint shaders[shader_count] = { 0 };
	// Textures for static meshes
	static constexpr uint32_t max_static_meshes = 8;
	GLuint static_mesh_textures[max_static_meshes] = { 0 };

	void load_border_data(parsers::scenario_building_context& context);
	void create_border_ogl_objects();
	void load_province_data(parsers::scenario_building_context& context, image& image);
	void load_provinces_mid_point(parsers::scenario_building_context& context);
	void load_terrain_data(parsers::scenario_building_context& context);
	void load_median_terrain_type(parsers::scenario_building_context& context);

	uint16_t safe_get_province(glm::ivec2 pt);
	void make_coastal_borders(sys::state& state, std::vector<bool>& visited);
	void make_borders(sys::state& state, std::vector<bool>& visited);

	void load_shaders(simple_fs::directory& root);
	void create_meshes();
	void gen_prov_color_texture(GLuint texture_handle, std::vector<uint32_t> const& prov_color, uint8_t layers = 1);

	void create_curved_river_vertices(parsers::scenario_building_context& context, std::vector<uint8_t> const& river_data, std::vector<uint8_t> const& terrain_data);
};

void load_river_crossings(parsers::scenario_building_context& context, std::vector<uint8_t> const& river_data, glm::ivec2 map_size);

void make_navy_path(sys::state& state, std::vector<map::curved_line_vertex>& buffer, dcon::navy_id selected_navy, float size_x, float size_y);
void make_army_path(sys::state& state, std::vector<map::curved_line_vertex>& buffer, dcon::army_id selected_army, float size_x, float size_y);
glm::vec2 put_in_local(glm::vec2 new_point, glm::vec2 base_point, float size_x);
void add_bezier_to_buffer(std::vector<map::curved_line_vertex>& buffer, glm::vec2 start, glm::vec2 end, glm::vec2 start_per, glm::vec2 end_per, float progress, bool last_curve, float size_x, float size_y, uint32_t num_b_segments);
void add_tl_bezier_to_buffer(std::vector<map::textured_line_vertex>& buffer, glm::vec2 start, glm::vec2 end, glm::vec2 start_per, glm::vec2 end_per, float progress, bool last_curve, float size_x, float size_y, uint32_t num_b_segments, float& distance);

image load_stb_image(simple_fs::file& file);
} // namespace map
