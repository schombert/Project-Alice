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

	image(uint8_t* data, int32_t size_x, int32_t size_y, int32_t channels) {
		this->data = data;
		this->size_x = size_x;
		this->size_y = size_y;
		this->channels = channels;
	}

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
struct border_vertex {
	border_vertex(){};
	border_vertex(glm::vec2 position, glm::vec2 normal_direction, glm::vec2 direction, int32_t border_id)
			: position_(position), normal_direction_(normal_direction), direction_(direction), border_id_(border_id){};
	glm::vec2 position_;
	glm::vec2 normal_direction_;
	glm::vec2 direction_;
	int32_t border_id_;
};
struct unit_arrow_vertex {
	unit_arrow_vertex(){};
	unit_arrow_vertex(glm::vec2 position, glm::vec2 normal_direction, glm::vec2 direction, glm::vec2 texture_coord, float type)
			: position_(position), normal_direction_(normal_direction), direction_(direction), texture_coord_(texture_coord), type_{type} {};
	glm::vec2 position_;
	glm::vec2 normal_direction_;
	glm::vec2 direction_;
	glm::vec2 texture_coord_;
	float type_;
};
struct border {
	int start_index = -1;
	int count = -1;
	uint8_t type_flag;
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

	void render(glm::vec2 screen_size, glm::vec2 offset, float zoom, map_view map_view_mode, map_mode::mode active_map_mode,
			glm::mat3 globe_rotation, float time_counter);
	void update_borders(sys::state& state);
	void set_selected_province(sys::state& state, dcon::province_id province_id);
	void set_province_color(std::vector<uint32_t> const& prov_color);
	void set_drag_box(bool draw_box, glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pixel_size);
	void set_unit_arrows(std::vector<std::vector<glm::vec2>> const& arrows);

	uint32_t size_x;
	uint32_t size_y;

	std::vector<border> borders;
	std::vector<border_vertex> border_vertices;
	std::vector<border_vertex> river_vertices;
	std::vector<unit_arrow_vertex> unit_arrow_vertices;
	std::vector<screen_vertex> drag_box_vertices;
	std::vector<uint8_t> terrain_id_map;
	std::vector<uint8_t> median_terrain_type;

	// map pixel -> province id
	std::vector<uint16_t> province_id_map;

private:
	// Meshes
	GLuint land_vao = 0;
	GLuint land_vbo = 0;
	GLuint border_vao = 0;
	GLuint border_vbo = 0;
	GLuint river_vao = 0;
	GLuint river_vbo = 0;
	GLuint unit_arrow_vao = 0;
	GLuint unit_arrow_vbo = 0;
	GLuint drag_box_vao = 0;
	GLuint drag_box_vbo = 0;
	uint32_t land_vertex_count = 0;

	// Textures
	GLuint provinces_texture_handle = 0;
	GLuint terrain_texture_handle = 0;
	GLuint terrainsheet_texture_handle = 0;
	GLuint water_normal = 0;
	GLuint colormap_water = 0;
	GLuint colormap_terrain = 0;
	GLuint colormap_political = 0;
	GLuint overlay = 0;
	GLuint province_color = 0;
	GLuint province_highlight = 0;
	GLuint stripes_texture = 0;
	GLuint unit_arrow_texture = 0;

	// Shaders
	GLuint terrain_shader = 0;
	GLuint line_border_shader = 0;
	GLuint line_river_shader = 0;
	GLuint line_unit_arrow_shader = 0;
	GLuint drag_box_shader = 0;

	void load_border_data(parsers::scenario_building_context& context);
	void create_border_ogl_objects();
	void load_province_data(parsers::scenario_building_context& context, image& image);
	void load_provinces_mid_point(parsers::scenario_building_context& context);
	void load_terrain_data(parsers::scenario_building_context& context);
	void load_median_terrain_type(parsers::scenario_building_context& context);

	void load_shaders(simple_fs::directory& root);
	void create_meshes();
	void gen_prov_color_texture(GLuint texture_handle, std::vector<uint32_t> const& prov_color, uint8_t layers = 1);
};

std::vector<border_vertex> create_river_vertices(display_data const& data, parsers::scenario_building_context& context, std::vector<uint8_t> river_data);

image load_stb_image(simple_fs::file& file);
} // namespace map
