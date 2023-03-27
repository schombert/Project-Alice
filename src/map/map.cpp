#include "map.hpp"
#include "map.hpp"
#include "texture.hpp"
#include "province.hpp"
#include <cmath>
#include <numbers>
#include <glm/glm.hpp>
#include <unordered_map>

namespace map {

image load_stb_image(simple_fs::file& file) {
	int32_t file_channels = 4;
	int32_t size_x = 0;
	int32_t size_y = 0;
	auto content = simple_fs::view_contents(file);
	auto data = stbi_load_from_memory(reinterpret_cast<uint8_t const*>(content.data), int32_t(content.file_size),
		&size_x, &size_y, &file_channels, 4);
	return image(data, size_x, size_y, 4);
}

GLuint make_gl_texture(uint8_t* data, uint32_t size_x, uint32_t size_y, uint32_t channels) {
	GLuint texture_handle;
	glGenTextures(1, &texture_handle);
	const GLuint internalformats[] = { GL_R8, GL_RG8, GL_RGB8, GL_RGBA8 };
	const GLuint formats[] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };
	if(texture_handle) {
		glBindTexture(GL_TEXTURE_2D, texture_handle);
		glTexStorage2D(
			GL_TEXTURE_2D,
			1,
			internalformats[channels - 1],
			size_x, size_y);
		glTexSubImage2D(
			GL_TEXTURE_2D,
			0, 0, 0,
			size_x, size_y,
			formats[channels - 1],
			GL_UNSIGNED_BYTE,
			data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	return texture_handle;
}
GLuint make_gl_texture(image& image) {
	return make_gl_texture(image.data, image.size_x, image.size_y, image.channels);
}

void set_gltex_parameters(GLuint texture_handle, GLuint texture_type, GLuint filter, GLuint wrap) {
	glBindTexture(texture_type, texture_handle);
	if(filter == GL_LINEAR_MIPMAP_NEAREST) {
		glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(texture_type);
	} else {
		glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, filter);
		glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, filter);
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
				glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
					0, 0,
					0, GLint(x * tiles_x + y),
					GLsizei(p_dx), GLsizei(p_dy),
					1,
					GL_RGBA,
					GL_UNSIGNED_BYTE,
					((uint32_t const*)image.data) + (x * p_dy * image.size_x + y * p_dx));

		set_gltex_parameters(texture_handle, GL_TEXTURE_2D_ARRAY, GL_LINEAR_MIPMAP_NEAREST, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, 0);
	}

	return texture_handle;
}

enum direction: uint8_t {
	UP_LEFT = 1 << 7,
	UP_RIGHT = 1 << 6,
	DOWN_LEFT = 1 << 5,
	DOWN_RIGHT = 1 << 4,
	UP = 1 << 3,
	DOWN = 1 << 2,
	LEFT = 1 << 1,
	RIGHT = 1 << 0,
};

struct BorderDirection {
	BorderDirection() {
	};
	struct Information {
		Information() {
		};
		Information(int32_t index_, int32_t id_): index{ index_ }, id{ id_ } {
		};
		int32_t index = -1;
		int32_t id = -1;
	};
	Information up;
	Information down;
	Information left;
	Information right;
};

void display_data::load_border_data(parsers::scenario_building_context& context) {
	border_vertices.clear();

	glm::vec2 map_size(size_x, size_y);

	std::vector<std::vector<border_vertex>> borders_list_vertices;

	// The borders of the current row and last row
	std::vector<BorderDirection> current_row(size_x);
	std::vector<BorderDirection> last_row(size_x);
	// Will check if there is an border there already and extend if it can
	auto extend_if_possible = [&](uint32_t x, int32_t border_id, direction dir) -> bool {
		if(dir == direction::LEFT)
			if(x == 0)
				return false;

		BorderDirection::Information direction_information;
		switch(dir) {
			case direction::UP:
				direction_information = last_row[x].down; break;
			case direction::DOWN:
				direction_information = current_row[x].up; break;
			case direction::LEFT:
				direction_information = current_row[x - 1].right; break;
			case direction::RIGHT:
				direction_information = current_row[x].left; break;
			default:
				return false;
		}
		if(direction_information.id != border_id)
			return false;

		auto border_index = direction_information.index;
		if(border_index == -1)
			return false;

		auto& current_border_vertices = borders_list_vertices[border_id];

		switch(dir) {
			case direction::UP:
			case direction::DOWN:
				current_border_vertices[border_index + 2].position_.y += 0.5f / map_size.y;
				current_border_vertices[border_index + 3].position_.y += 0.5f / map_size.y;
				current_border_vertices[border_index + 4].position_.y += 0.5f / map_size.y;
				break;
			case direction::LEFT:
			case direction::RIGHT:
				current_border_vertices[border_index + 2].position_.x += 0.5f / map_size.x;
				current_border_vertices[border_index + 3].position_.x += 0.5f / map_size.x;
				current_border_vertices[border_index + 4].position_.x += 0.5f / map_size.x;
				break;
			default:
				break;
		}
		switch(dir) {
			case direction::UP:
				current_row[x].up = direction_information; break;
			case direction::DOWN:
				current_row[x].down = direction_information; break;
			case direction::LEFT:
				current_row[x].left = direction_information; break;
			case direction::RIGHT:
				current_row[x].right = direction_information; break;
			default:
				break;
		}
		return true;
	};

	// Create a new vertices to make a line segment
	auto add_line = [&](glm::vec2 map_pos, glm::vec2 offset1, glm::vec2 offset2, int32_t border_id, uint32_t x, direction dir) {
		glm::vec2 direction = normalize(offset2 - offset1);
		glm::vec2 normal_direction = glm::vec2(-direction.y, direction.x);

		if(uint32_t(border_id) >= borders_list_vertices.size())
			borders_list_vertices.resize(border_id + 1);
		auto& current_border_vertices = borders_list_vertices[border_id];

		// Offset the map position
		map_pos += glm::vec2(0.5f);
		// Get the map coordinates
		glm::vec2 pos1 = offset1 + map_pos;
		glm::vec2 pos2 = offset2 + map_pos;

		// Rescale the coordinate to 0-1
		pos1 /= map_size;
		pos2 /= map_size;

		int32_t border_index = int32_t(current_border_vertices.size());
		// First vertex of the line segment
		current_border_vertices.emplace_back(pos1, normal_direction, direction, border_id);
		current_border_vertices.emplace_back(pos1, -normal_direction, direction, border_id);
		current_border_vertices.emplace_back(pos2, -normal_direction, -direction, border_id);
		// Second vertex of the line segment
		current_border_vertices.emplace_back(pos2, -normal_direction, -direction, border_id);
		current_border_vertices.emplace_back(pos2, normal_direction, -direction, border_id);
		current_border_vertices.emplace_back(pos1, normal_direction, direction, border_id);

		BorderDirection::Information direction_information(border_index, border_id);
		switch(dir) {
			case direction::UP:
				current_row[x].up = direction_information; break;
			case direction::DOWN:
				current_row[x].down = direction_information; break;
			case direction::LEFT:
				current_row[x].left = direction_information; break;
			case direction::RIGHT:
				current_row[x].right = direction_information; break;
			default:
				break;
		}
	};

	// Get the index of the border from the province ids and create a new one if one doesn't exist
	auto get_border_index = [&](uint16_t map_province_id1, uint16_t map_province_id2) -> int32_t {
		auto province_id1 = province::from_map_id(map_province_id1);
		auto province_id2 = province::from_map_id(map_province_id2);
		if (map_province_id1 == 3087 && map_province_id2 == 3088) {
			int i = 0;
		}
		auto border_index = context.state.world.get_province_adjacency_by_province_pair(province_id1, province_id2).index();
		if(border_index == -1)
			border_index = context.state.world.force_create_province_adjacency(province_id1, province_id2).index();
		return border_index;
	};

	auto add_border = [&](uint32_t x0, uint32_t y0, uint16_t id_ul, uint16_t id_ur, uint16_t id_dl, uint16_t id_dr) {
		// Yes this can be done better and more optimized. But for its good for now
		uint8_t diff_u = id_ul != id_ur;
		uint8_t diff_d = id_dl != id_dr;
		uint8_t diff_l = id_ul != id_dl;
		uint8_t diff_r = id_ur != id_dr;

		glm::vec2 map_pos(x0, y0);

		// Diagonal borders
		if(diff_l && diff_u && !diff_r && !diff_d) { // Upper left
			auto border_index = get_border_index(id_ul, id_dl);
			glm::vec2 pos1 = glm::vec2(0.0f, 0.5f);
			glm::vec2 pos2 = glm::vec2(0.5f, 0.0f);
			add_line(map_pos, pos1, pos2, border_index, x0, direction::UP_LEFT);
			return;
		}
		if(diff_l && diff_d && !diff_r && !diff_u) { // Lower left
			auto border_index = get_border_index(id_ul, id_dl);
			glm::vec2 pos1 = glm::vec2(0.0f, 0.5f);
			glm::vec2 pos2 = glm::vec2(0.5f, 1.0f);
			add_line(map_pos, pos1, pos2, border_index, x0, direction::DOWN_LEFT);
			return;
		}
		if(diff_r && diff_u && !diff_l && !diff_d) { // Upper right
			auto border_index = get_border_index(id_ur, id_dr);
			glm::vec2 pos1 = glm::vec2(1.0f, 0.5f);
			glm::vec2 pos2 = glm::vec2(0.5f, 0.0f);
			add_line(map_pos, pos1, pos2, border_index, x0, direction::UP_RIGHT);
			return;
		}
		if(diff_r && diff_d && !diff_l && !diff_u) { // Lower right
			auto border_index = get_border_index(id_ur, id_dr);
			glm::vec2 pos1 = glm::vec2(1.0f, 0.5f);
			glm::vec2 pos2 = glm::vec2(0.5f, 1.0f);
			add_line(map_pos, pos1, pos2, border_index, x0, direction::DOWN_LEFT);
			return;
		}
		// Straight borders
		if(diff_u) {
			auto border_index = get_border_index(id_ul, id_ur);
			glm::vec2 pos1 = glm::vec2(0.5f, 0.0f);
			glm::vec2 pos2 = glm::vec2(0.5f, 0.5f);
			if(!extend_if_possible(x0, border_index, direction::UP))
				add_line(map_pos, pos1, pos2, border_index, x0, direction::UP);
		}
		if(diff_d) {
			auto border_index = get_border_index(id_dl, id_dr);
			glm::vec2 pos1 = glm::vec2(0.5f, 0.5f);
			glm::vec2 pos2 = glm::vec2(0.5f, 1.0f);
			if(!extend_if_possible(x0, border_index, direction::DOWN))
				add_line(map_pos, pos1, pos2, border_index, x0, direction::DOWN);
		}
		if(diff_l) {
			auto border_index = get_border_index(id_ul, id_dl);
			glm::vec2 pos1 = glm::vec2(0.0f, 0.5f);
			glm::vec2 pos2 = glm::vec2(0.5f, 0.5f);
			if(!extend_if_possible(x0, border_index, direction::LEFT))
				add_line(map_pos, pos1, pos2, border_index, x0, direction::LEFT);
		}
		if(diff_r) {
			auto border_index = get_border_index(id_ur, id_dr);
			glm::vec2 pos1 = glm::vec2(0.5f, 0.5f);
			glm::vec2 pos2 = glm::vec2(1.0f, 0.5f);
			if(!extend_if_possible(x0, border_index, direction::LEFT))
				add_line(map_pos, pos1, pos2, border_index, x0, direction::RIGHT);
		}
	};

	for(uint32_t y = 0; y < size_y - 1; y++) {
		for(uint32_t x = 0; x < size_x - 1; x++) {
			auto prov_id_ul = province_id_map[(x + 0) + (y + 0) * size_x];
			auto prov_id_ur = province_id_map[(x + 1) + (y + 0) * size_x];
			auto prov_id_dl = province_id_map[(x + 0) + (y + 1) * size_x];
			auto prov_id_dr = province_id_map[(x + 1) + (y + 1) * size_x];
			if(prov_id_ul != prov_id_ur) {
				add_border(x, y, prov_id_ul, prov_id_ur, prov_id_dl, prov_id_dr);
				if(prov_id_ur != 0 && prov_id_ul != 0)
					context.state.world.try_create_province_adjacency(province::from_map_id(prov_id_ul), province::from_map_id(prov_id_ur));
			} else if(prov_id_ul != prov_id_dl) {
				add_border(x, y, prov_id_ul, prov_id_ur, prov_id_dl, prov_id_dr);
				if(prov_id_dl != 0 && prov_id_ul != 0)
					context.state.world.try_create_province_adjacency(province::from_map_id(prov_id_ul), province::from_map_id(prov_id_dl));
			} else if(prov_id_ul != prov_id_dr) {
				add_border(x, y, prov_id_ul, prov_id_ur, prov_id_dl, prov_id_dr);
				if(prov_id_dr != 0 && prov_id_ul != 0)
					context.state.world.try_create_province_adjacency(province::from_map_id(prov_id_ul), province::from_map_id(prov_id_dr));
			}
		}
		{
			// handle the international date line
			auto prov_id_ul = province_id_map[((size_x - 1) + 0) + (y + 0) * size_x];
			auto prov_id_ur = province_id_map[0 + (y + 0) * size_x];
			auto prov_id_dl = province_id_map[((size_x - 1) + 0) + (y + 1) * size_x];
			auto prov_id_dr = province_id_map[0 + (y + 1) * size_x];
			if(prov_id_ul != prov_id_ur) {
				add_border((size_x - 1), y, prov_id_ul, prov_id_ur, prov_id_dl, prov_id_dr);
				if(prov_id_ur != 0 && prov_id_ul != 0)
					context.state.world.try_create_province_adjacency(province::from_map_id(prov_id_ul), province::from_map_id(prov_id_ur));
			} else if(prov_id_ul != prov_id_dl) {
				add_border((size_x - 1), y, prov_id_ul, prov_id_ur, prov_id_dl, prov_id_dr);
				if(prov_id_dl != 0 && prov_id_ul != 0)
					context.state.world.try_create_province_adjacency(province::from_map_id(prov_id_ul), province::from_map_id(prov_id_dl));
			} else if(prov_id_ul != prov_id_dr) {
				add_border((size_x - 1), y, prov_id_ul, prov_id_ur, prov_id_dl, prov_id_dr);
				if(prov_id_dr != 0 && prov_id_ul != 0)
					context.state.world.try_create_province_adjacency(province::from_map_id(prov_id_ul), province::from_map_id(prov_id_dr));
			}
		}
		// Move the border_direction rows a step down
		for(uint32_t x = 0; x < size_x; x++) {
			last_row[x] = current_row[x];
			current_row[x] = BorderDirection();
		}
	}

	borders.resize(borders_list_vertices.size());
	for(uint32_t border_id = 0; border_id < borders.size(); border_id++) {
		auto& border = borders[border_id];
		auto& current_border_vertices = borders_list_vertices[border_id];
		border.start_index = int32_t(border_vertices.size());
		border.count = int32_t(current_border_vertices.size());
		border.type_flag = context.state.world.province_adjacency_get_type(dcon::province_adjacency_id(dcon::province_adjacency_id::value_base_t(border_id)));

		border_vertices.insert(border_vertices.end(),
			std::make_move_iterator(current_border_vertices.begin()),
			std::make_move_iterator(current_border_vertices.end()));
	}
}

void display_data::update_borders(sys::state& state) {
	for(uint32_t border_id = 0; border_id < borders.size(); border_id++) {
		auto& border = borders[border_id];
		border.type_flag = state.world.province_adjacency_get_type(dcon::province_adjacency_id(dcon::province_adjacency_id::value_base_t(border_id)));
	}
}

void setupVertexAttrib(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* offset) {
	glVertexAttribFormat(index, size, type, normalized, stride);
	glEnableVertexAttribArray(index);
	glVertexAttribBinding(index, 0);
}

void display_data::create_border_ogl_objects() {
	// Create and populate the VBO
	glGenBuffers(1, &border_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, border_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(border_vertex) * border_vertices.size(), &border_vertices[0], GL_STATIC_DRAW);

	// Create and bind the VAO
	glGenVertexArrays(1, &border_vao);
	glBindVertexArray(border_vao);

	// Bind the VBO to 0 of the VAO
	glBindVertexBuffer(0, border_vbo, 0, sizeof(border_vertex));

	// Set up vertex attribute format for the position
	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, offsetof(border_vertex, position_));
	// Set up vertex attribute format for the normal direction
	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, offsetof(border_vertex, normal_direction_));
	// Set up vertex attribute format for the direction
	glVertexAttribFormat(2, 2, GL_FLOAT, GL_FALSE, offsetof(border_vertex, direction_));
	// Set up vertex attribute format for the border id
	glVertexAttribFormat(3, 1, GL_UNSIGNED_INT, GL_FALSE, offsetof(border_vertex, border_id_));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glVertexAttribBinding(0, 0);
	glVertexAttribBinding(1, 0);
	glVertexAttribBinding(2, 0);
	glVertexAttribBinding(3, 0);

	glBindVertexArray(0);
}

void display_data::create_meshes() {

	std::vector<map_vertex> water_vertices;
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


	uint32_t index = 0;
	for(uint32_t y = 0; y < size_y; y++) {
		uint32_t last_x = 0;
		bool last_is_water = terrain_id_map[index++] > 64;
		for(uint32_t x = 1; x < size_x; x++) {
			bool is_water = terrain_id_map[index++] > 64;
			if(is_water != last_is_water || (x & (256 - 1)) == 0) {
				glm::vec2 pos0(last_x, y);
				glm::vec2 pos1(x, y + 1);
				if(last_is_water)
					add_quad(water_vertices, pos0, pos1);
				else
					add_quad(land_vertices, pos0, pos1);

				last_x = x;
				last_is_water = is_water;
			}
		}
		glm::vec2 pos0(last_x, y);
		glm::vec2 pos1(size_x, y + 1);
		if(last_is_water)
			add_quad(water_vertices, pos0, pos1);
		else
			add_quad(land_vertices, pos0, pos1);
	}

	water_vertex_count = ((uint32_t)water_vertices.size());
	land_vertex_count = ((uint32_t)land_vertices.size());

	// Create and populate the VBO
	glGenBuffers(1, &water_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, water_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(map_vertex) * water_vertices.size(), &water_vertices[0], GL_STATIC_DRAW);

	// Create and bind the VAO
	glGenVertexArrays(1, &water_vao);
	glBindVertexArray(water_vao);

	// Bind the VBO to 0 of the VAO
	glBindVertexBuffer(0, water_vbo, 0, sizeof(map_vertex));

	// Set up vertex attribute format for the position
	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, offsetof(map_vertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribBinding(0, 0);

	glBindVertexArray(0);

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
	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, offsetof(map_vertex, position));
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
	if(rivers_texture_handle)
		glDeleteTextures(1, &rivers_texture_handle);
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
	if(border_texture)
		glDeleteTextures(1, &border_texture);
	if(stripes_texture)
		glDeleteTextures(1, &stripes_texture);
	if(province_highlight)
		glDeleteTextures(1, &province_highlight);

	if(water_vao)
		glDeleteVertexArrays(1, &water_vao);
	if(land_vao)
		glDeleteVertexArrays(1, &land_vao);
	if(border_vao)
		glDeleteVertexArrays(1, &border_vao);

	if(land_vbo)
		glDeleteBuffers(1, &land_vbo);
	if(water_vbo)
		glDeleteBuffers(1, &water_vbo);
	if(border_vbo)
		glDeleteBuffers(1, &border_vbo);

	if(terrain_shader)
		glDeleteProgram(terrain_shader);
	if(terrain_political_close_shader)
		glDeleteProgram(terrain_political_close_shader);
	if(terrain_political_far_shader)
		glDeleteProgram(terrain_political_far_shader);
	if(water_shader)
		glDeleteProgram(water_shader);
	if(water_political_shader)
		glDeleteProgram(water_political_shader);
	if(line_border_shader)
		glDeleteProgram(line_border_shader);
	if(vic2_border_shader)
		glDeleteProgram(vic2_border_shader);
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
	auto map_vshader = try_load_shader(root, NATIVE("assets/shaders/map_v.glsl"));

	// Land shader
	auto map_fshader = try_load_shader(root, NATIVE("assets/shaders/map_f.glsl"));
	auto map_political_close_fshader = try_load_shader(root, NATIVE("assets/shaders/map_political_close_f.glsl"));
	auto map_political_far_fshader = try_load_shader(root, NATIVE("assets/shaders/map_political_far_f.glsl"));

	terrain_shader = create_program(*map_vshader, *map_fshader);
	terrain_political_close_shader = create_program(*map_vshader, *map_political_close_fshader);
	terrain_political_far_shader = create_program(*map_vshader, *map_political_far_fshader);

	// Water shaders
	auto map_water_fshader = try_load_shader(root, NATIVE("assets/shaders/map_water_f.glsl"));
	auto map_water_political_fshader = try_load_shader(root, NATIVE("assets/shaders/map_water_political_f.glsl"));

	water_shader = create_program(*map_vshader, *map_water_fshader);
	water_political_shader = create_program(*map_vshader, *map_water_political_fshader);

	// Line shaders
	auto line_border_vshader = try_load_shader(root, NATIVE("assets/shaders/line_border_v.glsl"));
	auto line_border_fshader = try_load_shader(root, NATIVE("assets/shaders/line_border_f.glsl"));

	auto vic2_border_vshader = try_load_shader(root, NATIVE("assets/shaders/vic2_border_v.glsl"));
	auto vic2_border_fshader = try_load_shader(root, NATIVE("assets/shaders/vic2_border_f.glsl"));

	line_border_shader = create_program(*line_border_vshader, *line_border_fshader);
	vic2_border_shader = create_program(*vic2_border_vshader, *vic2_border_fshader);
}

void display_data::render(sys::state& state, uint32_t screen_x, uint32_t screen_y) {
	update(state);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, provinces_texture_handle);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, terrain_texture_handle);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, rivers_texture_handle);
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

	auto load_shader = [&](GLuint program) {
		glUseProgram(program);

		// uniform vec2 offset
		glUniform2f(0, offset_x + 0.f, offset_y);
		// uniform float aspect_ratio
		glUniform1f(1, screen_x / ((float)screen_y));
		// uniform float zoom
		glUniform1f(2, zoom);
		// uniform vec2 map_size
		glUniform2f(3, GLfloat(size_x), GLfloat(size_y));
	};

	// Draw the land part of the map
	if(active_map_mode == map_mode::mode::terrain)
		load_shader(terrain_shader);
	else {
		if(zoom > 5)
			load_shader(terrain_political_close_shader);
		else
			load_shader(terrain_political_far_shader);
	}

	glBindVertexArray(land_vao);
	glDrawArrays(GL_TRIANGLES, 0, land_vertex_count);

	// Draw the water part of the map
	if(active_map_mode == map_mode::mode::terrain || zoom > 5) {
		load_shader(water_shader);
		// uniform float time
		glUniform1f(4, time_counter);
	} else {
		load_shader(water_political_shader);
	}

	glBindVertexArray(water_vao);
	glDrawArrays(GL_TRIANGLES, 0, water_vertex_count);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, border_texture);

	// Draw the borders
	load_shader(line_border_shader);
	glBindVertexArray(border_vao);


	if(zoom > 8) {
		glUniform1f(4, 0.0013f);
		uint8_t visible_borders =
			(province::border::national_bit
			| province::border::coastal_bit
			| province::border::non_adjacent_bit
			| province::border::impassible_bit
			| province::border::state_bit);

		std::vector<GLint> first;
		std::vector<GLsizei> count;
		for(auto& border : borders) {
			if((border.type_flag & visible_borders) == 0) {
				first.push_back(border.start_index);
				count.push_back(border.count);
			}
		}
		glMultiDrawArrays(GL_TRIANGLES, &first[0], &count[0], GLsizei(count.size()));
	}


	if(zoom > 3.5f) {
		glUniform1f(4, 0.0018f);
		uint8_t visible_borders =
			(province::border::national_bit
			| province::border::coastal_bit
			| province::border::non_adjacent_bit
			| province::border::impassible_bit);

		std::vector<GLint> first;
		std::vector<GLsizei> count;
		for(auto& border : borders) {
			if((border.type_flag & visible_borders) == 0 && (border.type_flag & province::border::state_bit) != 0) {
				first.push_back(border.start_index);
				count.push_back(border.count);
			}
		}
		glMultiDrawArrays(GL_TRIANGLES, &first[0], &count[0], GLsizei(count.size()));
	}

	{
		glUniform1f(4, 0.0027f);
		uint8_t visible_borders =
			(province::border::national_bit
			| province::border::coastal_bit
			| province::border::non_adjacent_bit
			| province::border::impassible_bit);

		std::vector<GLint> first;
		std::vector<GLsizei> count;
		for(auto& border : borders) {
			if(border.type_flag & visible_borders) {
				first.push_back(border.start_index);
				count.push_back(border.count);
			}
		}

		glMultiDrawArrays(GL_TRIANGLES, &first[0], &count[0], GLsizei(count.size()));
	}
	
	

	glBindVertexArray(0);
	glDisable(GL_CULL_FACE);
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


void display_data::set_province_color(std::vector<uint32_t> const& prov_color, map_mode::mode new_map_mode) {
	active_map_mode = new_map_mode;
	gen_prov_color_texture(province_color, prov_color, 2);
}

void display_data::set_terrain_map_mode() {
	active_map_mode = map_mode::mode::terrain;
}

void display_data::load_median_terrain_type(parsers::scenario_building_context& context) {
	median_terrain_type.resize(context.state.world.province_size() + 1);
	std::vector<std::array<int, 64>> terrain_histogram(context.state.world.province_size() + 1, std::array<int, 64>{});
	for(int i = size_x * size_y - 1; i-- > 0;) {
		auto prov_id = province_id_map[i];
		auto terrain_id = terrain_id_map[i];
		if(terrain_id < 64)
			terrain_histogram[prov_id][terrain_id] += 1;
	}

	for(int i = context.state.world.province_size(); i-- > 1;) { // map-id province 0 == the invalid province; we don't need to collect data for it
		int max_index = 64;
		int max = 0;
		for(int j = max_index; j-- > 0;) {
			if(terrain_histogram[i][j] > max) {
				max_index = j;
				max = terrain_histogram[i][j];
			}
		}
		median_terrain_type[i] = uint8_t(max_index);
	}
}

void display_data::load_terrain_data(parsers::scenario_building_context& context) {
	auto root = simple_fs::get_root(context.state.common_fs);
	auto map_dir = simple_fs::open_directory(root, NATIVE("map"));
	auto terrain_bmp = open_file(map_dir, NATIVE("terrain.bmp"));
	auto content = simple_fs::view_contents(*terrain_bmp);
	uint8_t* start = (uint8_t*)(content.data);

	// Data offset is where the pixel data starts
	uint8_t* ptr = start + 10;
	uint32_t data_offset = (ptr[3] << 24) | (ptr[2] << 16) | (ptr[1] << 8) | ptr[0];

	// The width & height of the image
	ptr = start + 18;
	uint32_t terrain_size_x = (ptr[3] << 24) | (ptr[2] << 16) | (ptr[1] << 8) | ptr[0];
	ptr = start + 22;
	uint32_t terrain_size_y = (ptr[3] << 24) | (ptr[2] << 16) | (ptr[1] << 8) | ptr[0];

	uint8_t* terrain_data = start + data_offset;

	// Create the terrain_id_map
	// If it is invalid province (meaning that the color didn't map to anything) or a sea province, it forces the terrain to be ocean.
	// If it is a land province, it forces the terrain to be plains if it is currently ocean
	terrain_id_map.resize(terrain_size_x * terrain_size_y, uint8_t(255));
	for(uint32_t y = 0; y < terrain_size_y; ++y) {
		for(uint32_t x = 0; x < terrain_size_x; ++x) {
			if(province_id_map[y * size_x + x] == 0 || province_id_map[y * size_x + x] >= province::to_map_id(context.state.province_definitions.first_sea_province)) {
				terrain_id_map[y * size_x + x] = uint8_t(255);
			} else {
				auto value = *(terrain_data + x + (terrain_size_y - y - 1) * terrain_size_x);
				if(value < 64)
					terrain_id_map[y * size_x + x] = value;
				else
					terrain_id_map[y * size_x + x] = uint8_t(6);
			}
		}
	}

	// Load the terrain
	load_median_terrain_type(context);
}

void display_data::load_provinces_mid_point(parsers::scenario_building_context& context) {
	std::vector<glm::ivec2> accumulated_tile_positions(context.state.world.province_size() + 1, glm::vec2(0));
	std::vector<int> tiles_number(context.state.world.province_size() + 1, 0);
	for(int i = size_x * size_y - 1; i-- > 0;) {
		auto prov_id = province_id_map[i];
		int x = i % size_x;
		int y = i / size_x;
		accumulated_tile_positions[prov_id] += glm::vec2(x, y);
		tiles_number[prov_id]++;
	}
	for(int i = context.state.world.province_size(); i-- > 1;) { // map-id province 0 == the invalid province; we don't need to collect data for it
		auto tile_pos = accumulated_tile_positions[i] / tiles_number[i];
		context.state.world.province_set_mid_point(province::from_map_id(uint16_t(i)), tile_pos);
	}
}

void display_data::load_province_data(parsers::scenario_building_context& context, image& image) {
	uint32_t imsz = uint32_t(image.size_x * image.size_y);
	province_id_map.resize(imsz);
	for(uint32_t i = 0; i < imsz; ++i) {
		uint8_t* ptr = image.data + i * 4;
		auto color = sys::pack_color(ptr[0], ptr[1], ptr[2]);
		if(auto it = context.map_color_to_province_id.find(color); it != context.map_color_to_province_id.end()) {
			province_id_map[i] = province::to_map_id(it->second);
		} else {
			province_id_map[i] = 0;
		}
	}

	load_provinces_mid_point(context);
}

void display_data::load_map_data(parsers::scenario_building_context& context) {
	auto root = simple_fs::get_root(context.state.common_fs);
	auto map_dir = simple_fs::open_directory(root, NATIVE("map"));

	// Load the province map
	auto provinces_bmp = open_file(map_dir, NATIVE("provinces.bmp"));
	auto provinces_image = load_stb_image(*provinces_bmp);

	size_x = uint32_t(provinces_image.size_x);
	size_y = uint32_t(provinces_image.size_y);

	load_province_data(context, provinces_image);
	load_terrain_data(context);
	load_border_data(context);

	parsers::error_handler err("adjacencies.csv");
	auto adj_csv_file = open_file(map_dir, NATIVE("adjacencies.csv"));
	if(adj_csv_file) {
		auto adj_content = view_contents(*adj_csv_file);
		parsers::read_map_adjacency(adj_content.data, adj_content.data + adj_content.file_size, err, context);
	}
}

GLuint load_dds_texture(simple_fs::directory const& dir, native_string_view file_name) {
	auto file = simple_fs::open_file(dir, file_name);
	auto content = simple_fs::view_contents(*file);
	uint32_t size_x, size_y;
	uint8_t const* data = (uint8_t const*)(content.data);
	return ogl::SOIL_direct_load_DDS_from_memory(data, content.file_size, size_x, size_y, ogl::SOIL_FLAG_TEXTURE_REPEATS);
}

void display_data::load_map(sys::state& state) {
	auto root = simple_fs::get_root(state.common_fs);
	auto map_dir = simple_fs::open_directory(root, NATIVE("map"));
	auto map_terrain_dir = simple_fs::open_directory(map_dir, NATIVE("terrain"));

	load_shaders(root);

	terrain_texture_handle = make_gl_texture(&terrain_id_map[0], size_x, size_y, 1);
	set_gltex_parameters(terrain_texture_handle, GL_TEXTURE_2D, GL_NEAREST, GL_CLAMP_TO_EDGE);
	create_meshes();

	provinces_texture_handle = load_province_map(province_id_map, size_x, size_y);

	auto rivers_bmp = open_file(map_dir, NATIVE("rivers.bmp"));
	auto rivers_image = load_stb_image(*rivers_bmp);
	rivers_texture_handle = make_gl_texture(rivers_image);
	set_gltex_parameters(rivers_texture_handle, GL_TEXTURE_2D, GL_NEAREST, GL_CLAMP_TO_EDGE);

	auto texturesheet = open_file(map_terrain_dir, NATIVE("texturesheet.tga"));
	terrainsheet_texture_handle = load_texture_array_from_file(*texturesheet, 8, 8);

	water_normal = load_dds_texture(map_terrain_dir, NATIVE("sea_normal.dds"));
	colormap_water = load_dds_texture(map_terrain_dir, NATIVE("colormap_water.dds"));
	colormap_terrain = load_dds_texture(map_terrain_dir, NATIVE("colormap.dds"));
	colormap_political = load_dds_texture(map_terrain_dir, NATIVE("colormap_political.dds"));
	overlay = load_dds_texture(map_terrain_dir, NATIVE("map_overlay_tile.dds"));
	border_texture = load_dds_texture(map_terrain_dir, NATIVE("borders.dds"));
	stripes_texture = load_dds_texture(map_terrain_dir, NATIVE("stripes.dds"));
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

	set_province_color(testColor, map_mode::mode::terrain);
}

void display_data::update(sys::state& state) {
	std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
	// Set the last_update_time if it hasn't been set yet
	if(last_update_time == std::chrono::time_point<std::chrono::system_clock>{})
		last_update_time = now;
	if(last_zoom_time == std::chrono::time_point<std::chrono::system_clock>{})
		last_zoom_time = now;

	auto microseconds_since_last_update = std::chrono::duration_cast<std::chrono::microseconds>(now - last_update_time);
	float seconds_since_last_update = (float)(microseconds_since_last_update.count() / 1e6);
	last_update_time = now;

	time_counter += seconds_since_last_update;
	time_counter = (float)std::fmod(time_counter, 600.f); // Reset it after every 10 minutes

	glm::vec2 velocity;

	velocity = (pos_velocity + scroll_pos_velocity) * (seconds_since_last_update / zoom);
	velocity.x *= float(size_y) / float(size_x);
	pos += velocity;

	if(has_zoom_changed) {
		last_zoom_time = now;
		has_zoom_changed = false;
	}
	auto microseconds_since_last_zoom = std::chrono::duration_cast<std::chrono::microseconds>(now - last_zoom_time);
	float seconds_since_last_zoom = (float)(microseconds_since_last_zoom.count() / 1e6);

	zoom += (zoom_change * seconds_since_last_update) / (1 / zoom);
	zoom_change *= std::max(0.1f - seconds_since_last_zoom, 0.f) * 9.5f;
	scroll_pos_velocity *= std::max(0.1f - seconds_since_last_zoom, 0.f) * 9.5f;

	pos.x = glm::mod(pos.x, 1.f);
	pos.y = glm::clamp(pos.y, 0.f, 1.f);
	offset_x = glm::mod(pos.x, 1.f) - 0.5f;
	offset_y = pos.y - 0.5f;

	if(unhandled_province_selection) {
		map_mode::update_map_mode(state);
		std::vector<uint32_t> province_highlights(state.world.province_size() + 1);
		if(selected_province)
			province_highlights[province::to_map_id(selected_province)] = 0x2B2B2B2B;
		gen_prov_color_texture(province_highlight, province_highlights);
		unhandled_province_selection = false;
	}
}

void display_data::on_key_down(sys::virtual_key keycode, sys::key_modifiers mod) {
	if(keycode == sys::virtual_key::LEFT) {
		pos_velocity.x = -1.f;
		left_arrow_key_down = true;
	} else if(keycode == sys::virtual_key::RIGHT) {
		pos_velocity.x = +1.f;
		right_arrow_key_down = true;
	} else if(keycode == sys::virtual_key::UP) {
		pos_velocity.y = -1.f;
		up_arrow_key_down = true;
	} else if(keycode == sys::virtual_key::DOWN) {
		pos_velocity.y = +1.f;
		down_arrow_key_down = true;
	} else if(keycode == sys::virtual_key::Q) {
		zoom_change = zoom * 1.1f;
	} else if(keycode == sys::virtual_key::E) {
		zoom_change = zoom * 0.9f;
	}
}

void display_data::on_key_up(sys::virtual_key keycode, sys::key_modifiers mod) {
	if(keycode == sys::virtual_key::LEFT) {
		if(pos_velocity.x < 0) {
			if(right_arrow_key_down == false) {
				pos_velocity.x = 0;
			} else {
				pos_velocity.x *= -1;
			}
		}
		left_arrow_key_down = false;
	} else if(keycode == sys::virtual_key::RIGHT) {
		if(pos_velocity.x > 0) {
			if(left_arrow_key_down == false) {
				pos_velocity.x = 0;
			} else {
				pos_velocity.x *= -1;
			}
		}
		right_arrow_key_down = false;
	} else if(keycode == sys::virtual_key::UP) {
		if(pos_velocity.y < 0) {
			if(down_arrow_key_down == false) {
				pos_velocity.y = 0;
			} else {
				pos_velocity.y *= -1;
			}
		}
		up_arrow_key_down = false;
	} else if(keycode == sys::virtual_key::DOWN) {
		if(pos_velocity.y > 0) {
			if(up_arrow_key_down == false) {
				pos_velocity.y = 0;
			} else {
				pos_velocity.y *= -1;
			}
		}
		down_arrow_key_down = false;
	}
}


void display_data::set_pos(glm::vec2 new_pos) {
	pos.x = glm::mod(new_pos.x, 1.f);
	pos.y = glm::clamp(new_pos.y, 0.f, 1.0f);
	offset_x = glm::mod(pos.x, 1.f) - 0.5f;
	offset_y = pos.y - 0.5f;
}

void display_data::on_mouse_wheel(int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod, float amount) {
	constexpr auto zoom_speed_factor = 15.f;
	zoom_change = std::copysign(((amount / 5.f) * zoom_speed_factor), amount);
	has_zoom_changed = true;

	auto mouse_pos = glm::vec2(x, y);
	auto screen_size = glm::vec2(screen_size_x, screen_size_y);
	scroll_pos_velocity = mouse_pos - screen_size * .5f;
	scroll_pos_velocity /= screen_size;
	scroll_pos_velocity *= zoom_speed_factor;
	if(zoom_change > 0) {
		scroll_pos_velocity /= 3.f;
	} else if(zoom_change < 0) {
		scroll_pos_velocity /= 6.f;
	}
}

void display_data::on_mouse_move(int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod) {
	if(is_dragging) {  // Drag the map with middlemouse
		auto mouse_pos = glm::vec2(x, y);
		auto screen_size = glm::vec2(screen_size_x, screen_size_y);
		auto map_pos = screen_to_map(mouse_pos, screen_size);

		set_pos(pos + last_camera_drag_pos - glm::vec2(map_pos));
	}
}

glm::vec2 display_data::screen_to_map(glm::vec2 screen_pos, glm::vec2 screen_size) {
	screen_pos -= screen_size * 0.5f;
	screen_pos /= screen_size;
	screen_pos.x *= screen_size.x / screen_size.y;
	screen_pos.x *= float(size_y) / float(size_x);

	screen_pos /= zoom;
	screen_pos += pos;
	return screen_pos;
}

void display_data::on_mbuttom_down(int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod) {
	auto mouse_pos = glm::vec2(x, y);
	auto screen_size = glm::vec2(screen_size_x, screen_size_y);

	auto map_pos = screen_to_map(mouse_pos, screen_size);

	last_camera_drag_pos = map_pos;
	is_dragging = true;
	pos_velocity = glm::vec2(0);
}

void display_data::on_mbuttom_up(int32_t x, int32_t y, sys::key_modifiers mod) {
	is_dragging = false;
}

void display_data::on_lbutton_down(sys::state& state, int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod) {
	auto mouse_pos = glm::vec2(x, y);
	auto screen_size = glm::vec2(screen_size_x, screen_size_y);
	auto map_pos = screen_to_map(mouse_pos, screen_size);
	map_pos *= glm::vec2(float(size_x), float(size_y));
	auto idx = int32_t(size_y - map_pos.y) * int32_t(size_x) + int32_t(map_pos.x);
	if(0 <= idx && size_t(idx) < province_id_map.size()) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
		auto fat_id = dcon::fatten(state.world, province::from_map_id(province_id_map[idx]));
		if(province_id_map[idx] < province::to_map_id(state.province_definitions.first_sea_province)) {
			set_selected_province(province::from_map_id(province_id_map[idx]));
		} else {
			set_selected_province(dcon::province_id{});
		}
	} else {
		set_selected_province(dcon::province_id{});
	}
}

dcon::province_id display_data::get_selected_province() {
	return selected_province;
}

void display_data::set_selected_province(dcon::province_id prov_id) {
	unhandled_province_selection = selected_province != prov_id;
	selected_province = prov_id;
}
}
