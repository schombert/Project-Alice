#include "map.hpp"
#include "texture.hpp"
#include "province.hpp"
#include <cmath>
#include <glm/glm.hpp>
#include <fstream>

namespace map {
void set_gltex_parameters(GLuint texture_type, GLuint filter, GLuint wrap) {
	if(filter == GL_LINEAR_MIPMAP_NEAREST) {
		glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(texture_type);
	} else {
		glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, filter);
		glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, filter);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
}

GLuint load_texture_from_file(simple_fs::file& file, GLuint filter) {
	auto content = simple_fs::view_contents(file);
	int32_t file_channels, size_x, size_y;

	auto data = stbi_load_from_memory(reinterpret_cast<uint8_t const*>(content.data), int32_t(content.file_size),
		&size_x, &size_y, &file_channels, 4);

	GLuint texture_handle;
	glGenTextures(1, &texture_handle);
	if(texture_handle) {
		glBindTexture(GL_TEXTURE_2D, texture_handle);

		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, size_x, size_y);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size_x, size_y, GL_RGBA, GL_UNSIGNED_BYTE, data);
		set_gltex_parameters(GL_TEXTURE_2D, filter, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	STBI_FREE(data);
	return texture_handle;
}

GLuint load_texture_array_from_file(sys::state& state, simple_fs::file& file, int32_t tiles_x, int32_t tiles_y) {
	auto content = simple_fs::view_contents(file);
	int32_t file_channels, size_x, size_y;

	auto data = stbi_load_from_memory(reinterpret_cast<uint8_t const*>(content.data), int32_t(content.file_size),
		&size_x, &size_y, &file_channels, 4);

	GLuint texture_handle;
	glGenTextures(1, &texture_handle);
	if(texture_handle) {
		glBindTexture(GL_TEXTURE_2D_ARRAY, texture_handle);

		size_t p_dx = size_x / tiles_x; // Pixels of each tile in x
		size_t p_dy = size_y / tiles_y; // Pixels of each tile in y
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, GLsizei(p_dx), GLsizei(p_dy), GLsizei(tiles_x * tiles_y), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, size_x);
		glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, size_y);

		for(int32_t x = 0; x < tiles_x; x++)
			for(int32_t y = 0; y < tiles_y; y++)
				glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
					0, 0,
					0, GLint(x * tiles_x + y),
					GLsizei(p_dx), GLsizei(p_dy),
					1,
					GL_RGBA,
					GL_UNSIGNED_BYTE,
					((uint32_t const*)data) + (x * p_dy * size_x + y * p_dx));

		set_gltex_parameters(GL_TEXTURE_2D_ARRAY, GL_LINEAR_MIPMAP_NEAREST, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, 0);
	}

	STBI_FREE(data);

	return texture_handle;
}

// Load the terrain texture, will read the BMP file directly
// The image is flipped for some reason
GLuint load_terrain_texture(simple_fs::file& file, int32_t& size_x, int32_t& size_y) {
	auto content = simple_fs::view_contents(file);
	uint8_t* start = (uint8_t*)(content.data);

	// TODO make a check for when the bmp format is unsupporeted

	// Data offset is where the pixel data starts
	uint8_t* ptr = start + 10;
	uint32_t data_offset = (ptr[3] << 24) | (ptr[2] << 16) | (ptr[1] << 8) | ptr[0];

	// The width & height of the image
	ptr = start + 18;
	size_x = (ptr[3] << 24) | (ptr[2] << 16) | (ptr[1] << 8) | ptr[0];
	ptr = start + 22;
	size_y = (ptr[3] << 24) | (ptr[2] << 16) | (ptr[1] << 8) | ptr[0];

	char const* data = content.data + data_offset;

	GLuint texture_handle;
	glGenTextures(1, &texture_handle);
	if(texture_handle) {
		glBindTexture(GL_TEXTURE_2D, texture_handle);

		// Create a texture with only one byte color
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, size_x, size_y);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size_x, size_y, GL_RED, GL_UNSIGNED_BYTE, data);
		set_gltex_parameters(GL_TEXTURE_2D, GL_NEAREST, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	return texture_handle;
}

GLuint load_dds_texture(simple_fs::directory const& dir, native_string_view file_name) {
	auto file = simple_fs::open_file(dir, file_name);
	auto content = simple_fs::view_contents(*file);
	uint32_t size_x, size_y;
	uint8_t const* data = (uint8_t const*)(content.data);
	return ogl::SOIL_direct_load_DDS_from_memory(data, content.file_size, size_x, size_y, ogl::SOIL_FLAG_TEXTURE_REPEATS);
}

void display_data::create_meshes(simple_fs::file& file) {
	auto content = simple_fs::view_contents(file);
	uint8_t* start = (uint8_t*)(content.data);

	// TODO make a check for when the bmp format is unsupporeted

	// Data offset is where the pixel data starts
	uint8_t* ptr = start + 10;
	uint32_t data_offset = (ptr[3] << 24) | (ptr[2] << 16) | (ptr[1] << 8) | ptr[0];
	ptr = start + 18;
	uint32_t size_x = (ptr[3] << 24) | (ptr[2] << 16) | (ptr[1] << 8) | ptr[0];
	ptr = start + 22;
	uint32_t size_y = (ptr[3] << 24) | (ptr[2] << 16) | (ptr[1] << 8) | ptr[0];

	uint8_t* terrain_data = start + data_offset;

	std::vector<float> water_vertices;
	std::vector<float> land_vertices;

	auto add_quad = [&map_size = this->size](std::vector<float>& vertices, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1) {
		vertices.push_back((float)(x0 / map_size.x));
		vertices.push_back((float)(1 - y0 / map_size.y));

		vertices.push_back((float)(x1 / map_size.x));
		vertices.push_back((float)(1 - y0 / map_size.y));

		vertices.push_back((float)(x1 / map_size.x));
		vertices.push_back((float)(1 - y1 / map_size.y));

		vertices.push_back((float)(x1 / map_size.x));
		vertices.push_back((float)(1 - y1 / map_size.y));

		vertices.push_back((float)(x0 / map_size.x));
		vertices.push_back((float)(1 - y1 / map_size.y));

		vertices.push_back((float)(x0 / map_size.x));
		vertices.push_back((float)(1 - y0 / map_size.y));
	};


	uint32_t index = 0;
	for(uint32_t y = 0; y < size_y; y++) {
		uint32_t last_x = 0;
		bool last_is_water = terrain_data[index++] > 64;
		for(uint32_t x = 1; x < size_x; x++) {
			bool is_water = terrain_data[index++] > 64;
			if(is_water != last_is_water) {
				if(last_is_water)
					add_quad(water_vertices, last_x, y, x, y + 1);
				else
					add_quad(land_vertices, last_x, y, x, y + 1);

				last_x = x;
				last_is_water = is_water;
			}
		}
		if(last_is_water)
			add_quad(water_vertices, last_x, y, size_x, y + 1);
		else
			add_quad(land_vertices, last_x, y, size_x, y + 1);
	}

	water_indicies = ((uint32_t)water_vertices.size()) / 3 * 2;
	land_indicies = ((uint32_t)land_vertices.size()) / 3 * 2;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &water_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, water_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * water_vertices.size(), &water_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &land_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, land_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * land_vertices.size(), &land_vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
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
	if(province_highlight)
		glDeleteTextures(1, &province_highlight);

	if(vao)
		glDeleteVertexArrays(1, &vao);
	if(land_vbo)
		glDeleteBuffers(1, &land_vbo);
	if(water_vbo)
		glDeleteBuffers(1, &water_vbo);

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
}

std::optional<simple_fs::file> try_load_shader(simple_fs::directory& root, native_string_view name) {
	auto shader = simple_fs::open_file(root, name);
	if(!bool(shader))
		ogl::notify_user_of_fatal_opengl_error("Unable to open a necessary shader file");
	return shader;
}

std::string_view get_content(simple_fs::file& file) {
	auto content = simple_fs::view_contents(file);
	return std::string_view(content.data, content.file_size);
}

void display_data::load_shaders(simple_fs::directory& root) {
	auto map_vshader = try_load_shader(root, NATIVE("assets/shaders/map_v.glsl"));

	auto map_fshader = try_load_shader(root, NATIVE("assets/shaders/map_f.glsl"));
	auto map_political_close_fshader = try_load_shader(root, NATIVE("assets/shaders/map_political_close_f.glsl"));
	auto map_political_far_fshader = try_load_shader(root, NATIVE("assets/shaders/map_political_far_f.glsl"));

	auto map_water_fshader = try_load_shader(root, NATIVE("assets/shaders/map_water_f.glsl"));
	auto map_water_political_fshader = try_load_shader(root, NATIVE("assets/shaders/map_water_political_f.glsl"));

	terrain_shader = ogl::create_program(
		get_content(*map_vshader), get_content(*map_fshader));
	terrain_political_close_shader = ogl::create_program(
		get_content(*map_vshader), get_content(*map_political_close_fshader));
	terrain_political_far_shader = ogl::create_program(
		get_content(*map_vshader), get_content(*map_political_far_fshader));

	water_shader = ogl::create_program(
		get_content(*map_vshader), get_content(*map_water_fshader));
	water_political_shader = ogl::create_program(
		get_content(*map_vshader), get_content(*map_water_political_fshader));
}

void display_data::render(uint32_t screen_x, uint32_t screen_y) {
	update();
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
	glBindTexture(GL_TEXTURE_2D, province_color);
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, colormap_political);
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, province_highlight);

	glBindVertexArray(vao);

	if (active_map_mode == map_mode::mode::terrain)
		glUseProgram(terrain_shader);
	else {
		if (zoom > 5)
			glUseProgram(terrain_political_close_shader);
		else
			glUseProgram(terrain_political_far_shader);
	}

	// uniform float aspect_ratio
	glUniform1f(1, screen_x / ((float)screen_y));
	// uniform float zoom
	glUniform1f(2, zoom);
	// uniform float time
	glUniform1f(4, time_counter);
	// uniform vec2 map_size
	glUniform2f(3, GLfloat(size.x), GLfloat(size.y));

	glBindVertexBuffer(0, land_vbo, 0, sizeof(GLfloat) * 2);

	glUniform2f(0, offset_x - 1.f, offset_y);
	glDrawArrays(GL_TRIANGLES, 0, land_indicies);
	glUniform2f(0, offset_x + 0.f, offset_y);
	glDrawArrays(GL_TRIANGLES, 0, land_indicies);
	glUniform2f(0, offset_x + 1.f, offset_y);
	glDrawArrays(GL_TRIANGLES, 0, land_indicies);

	if (active_map_mode == map_mode::mode::terrain)
		glUseProgram(water_shader);
	else {
		if (zoom > 5)
			glUseProgram(water_shader);
		else
			glUseProgram(water_political_shader);
	}

	glUniform2f(0, offset_x, offset_y);
	// uniform float aspect_ratio
	glUniform1f(1, screen_x / ((float)screen_y));
	// uniform float zoom
	glUniform1f(2, zoom);
	// uniform float time
	glUniform1f(4, time_counter);
	// uniform vec2 map_size
	glUniform2f(3, GLfloat(size.x), GLfloat(size.y));

	glBindVertexBuffer(0, water_vbo, 0, sizeof(GLfloat) * 2);

	glUniform2f(0, offset_x - 1.f, offset_y);
	glDrawArrays(GL_TRIANGLES, 0, water_indicies);
	glUniform2f(0, offset_x + 0.f, offset_y);
	glDrawArrays(GL_TRIANGLES, 0, water_indicies);
	glUniform2f(0, offset_x + 1.f, offset_y);
	glDrawArrays(GL_TRIANGLES, 0, water_indicies);
}

GLuint display_data::load_province_map(simple_fs::directory& map_dir, int& nr_of_province) {
	auto defs = simple_fs::open_file(map_dir, NATIVE("definition.csv"));

	auto defs_content = simple_fs::view_contents(*defs);
	char const* cpos = defs_content.data;
	char const* end_of_file = defs_content.data + defs_content.file_size;

	cpos = parsers::csv_advance_to_next_line(cpos, end_of_file);

	ankerl::unordered_dense::map<int32_t, int16_t> color_to_id;

	int16_t index = 0;
	parsers::error_handler err("no_file");
	while(cpos < end_of_file) {
		cpos = parsers::parse_fixed_amount_csv_values<6>(cpos, end_of_file, ';',
			[&](std::string_view const* values) {
				int32_t color = 0;
				int32_t color_part;
				color_part = parsers::parse_int(parsers::remove_surrounding_whitespace(values[1]), 0, err);
				color += color_part << 16; 	// Red
				color_part = parsers::parse_int(parsers::remove_surrounding_whitespace(values[2]), 0, err);
				color += color_part << 8; 	// Green
				color_part = parsers::parse_int(parsers::remove_surrounding_whitespace(values[3]), 0, err);
				color += color_part; 		// Blue
				color_to_id.insert_or_assign(color, index++);
			});
	}
	nr_of_province = index;

	auto provinces_bmp = open_file(map_dir, NATIVE("provinces.bmp"));
	auto bmp_content = simple_fs::view_contents(*provinces_bmp);
	int32_t file_channels, size_x, size_y;

	auto data = stbi_load_from_memory(reinterpret_cast<uint8_t const*>(bmp_content.data), int32_t(bmp_content.file_size),
		&size_x, &size_y, &file_channels, 4);

	std::vector<int16_t> province_ids(size_x * size_y);
	for(int i = 0; i < size_x * size_y; i++) {
		uint8_t* ptr = data + i * 4;
		int b = ptr[2];
		int g = ptr[1];
		int r = ptr[0];
		int32_t color = (r << 16) | (g << 8) | b;

		province_ids[i] = color_to_id[color] + 1;
	}
	province_id_map = province_ids;
	STBI_FREE(data);

	GLuint texture_handle;
	glGenTextures(1, &texture_handle);
	if(texture_handle) {
		glBindTexture(GL_TEXTURE_2D, texture_handle);

		// Create a texture with only one byte color
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG8, size_x, size_y);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size_x, size_y, GL_RG, GL_UNSIGNED_BYTE, &province_ids[0]);
		set_gltex_parameters(GL_TEXTURE_2D, GL_NEAREST, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	return texture_handle;
}

void display_data::gen_prov_color_texture(GLuint texture_handle, std::vector<uint32_t> const& prov_color) {
	glBindTexture(GL_TEXTURE_2D, texture_handle);
	uint32_t rows = ((uint32_t)prov_color.size()) / 256;
	uint32_t left_on_last_row = ((uint32_t)prov_color.size()) % 256;

	uint32_t x = 0, y = 0;
	uint32_t width = 256, height = rows;
	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, &prov_color[0]);
	x = 0, y = rows;
	width = left_on_last_row, height = 1;
	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, &prov_color[rows * 256]);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void display_data::set_province_color(std::vector<uint32_t> const& prov_color, map_mode::mode new_map_mode) {
	active_map_mode = new_map_mode;
	gen_prov_color_texture(province_color, prov_color);
}

void display_data::set_terrain_map_mode() {
	active_map_mode = map_mode::mode::terrain;
}

void display_data::load_map(sys::state& state) {
	auto root = simple_fs::get_root(state.common_fs);
	auto map_dir = simple_fs::open_directory(root, NATIVE("map"));
	auto map_terrain_dir = simple_fs::open_directory(map_dir, NATIVE("terrain"));

	// display_data& map_display = state.map_display;
	load_shaders(root);

	auto terrain_bmp = open_file(map_dir, NATIVE("terrain.bmp"));
	int32_t size_x, size_y;
	terrain_texture_handle = load_terrain_texture(*terrain_bmp, size_x, size_y);
	size = glm::vec2(size_x, size_y);
	create_meshes(*terrain_bmp);

	// TODO Better error handling and reporting ^^
	provinces_texture_handle = load_province_map(map_dir, nr_of_provinces);

	auto rivers_bmp = open_file(map_dir, NATIVE("rivers.bmp"));
	rivers_texture_handle = load_texture_from_file(*rivers_bmp, GL_NEAREST);
	auto texturesheet = open_file(map_terrain_dir, NATIVE("texturesheet.tga"));
	terrainsheet_texture_handle = load_texture_array_from_file(state, *texturesheet, 8, 8);

	water_normal = load_dds_texture(map_terrain_dir, NATIVE("sea_normal.dds"));
	colormap_water = load_dds_texture(map_terrain_dir, NATIVE("colormap_water.dds"));
	colormap_terrain = load_dds_texture(map_terrain_dir, NATIVE("colormap.dds"));
	colormap_political = load_dds_texture(map_terrain_dir, NATIVE("colormap_political.dds"));
	overlay = load_dds_texture(map_terrain_dir, NATIVE("map_overlay_tile.dds"));

	// Get the province_color handle
	glGenTextures(1, &province_color);
	glBindTexture(GL_TEXTURE_2D, province_color);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 256, 256);
	set_gltex_parameters(GL_TEXTURE_2D, GL_NEAREST, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Get the province_highlight handle
	glGenTextures(1, &province_highlight);
	glBindTexture(GL_TEXTURE_2D, province_highlight);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 256, 256);
	set_gltex_parameters(GL_TEXTURE_2D, GL_NEAREST, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	std::vector<uint32_t> test(nr_of_provinces);
	gen_prov_color_texture(province_highlight, test);
	for(int i = 0; i < nr_of_provinces; i++) {
		test[i] = 255;
	}
	set_province_color(test, map_mode::mode::terrain);
}

void display_data::update() {
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

	if(zoom_change > 0) {
		velocity = ((pos_velocity + scroll_pos_velocity) * (seconds_since_last_update / zoom)) / 3.f;
	}
	else {
		velocity = ((pos_velocity + scroll_pos_velocity) * (seconds_since_last_update / zoom)) / 6.f;
	}
	velocity.x *= size.y / size.x;
	pos += velocity;

	if(has_zoom_changed) {
		last_zoom_time = now;
		has_zoom_changed = false;
	}
	auto microseconds_since_last_zoom = std::chrono::duration_cast<std::chrono::microseconds>(now - last_zoom_time);
	float seconds_since_last_zoom = (float)(microseconds_since_last_zoom.count() / 1e6);

	zoom += (zoom_change * seconds_since_last_update)/(1/zoom);
	zoom_change *= std::max(0.1f - seconds_since_last_zoom, 0.f) * 9.5f;
	scroll_pos_velocity *= std::max(0.1f - seconds_since_last_zoom, 0.f) * 9.5f;

	pos.x = glm::mod(pos.x, 1.f);
	pos.y = glm::clamp(pos.y, 0.f, 1.f);
	offset_x = glm::mod(pos.x, 1.f) - 0.5f;
	offset_y = pos.y - 0.5f;

	if(unhandled_province_selection) {
		std::vector<uint32_t> province_highlights(nr_of_provinces);
		if(selected_province)
			province_highlights[selected_province] = 0x2B2B2B2B;
		gen_prov_color_texture(province_highlight, province_highlights);
		unhandled_province_selection = false;
	}
}

void display_data::on_key_down(sys::virtual_key keycode, sys::key_modifiers mod) {
	if(keycode == sys::virtual_key::LEFT) {
		pos_velocity.x = -1.f;
	} else if(keycode == sys::virtual_key::RIGHT) {
		pos_velocity.x = +1.f;
	} else if(keycode == sys::virtual_key::UP) {
		pos_velocity.y = -1.f;
	} else if(keycode == sys::virtual_key::DOWN) {
		pos_velocity.y = +1.f;
	} else if(keycode == sys::virtual_key::Q) {
		zoom_change = zoom * 1.1f;
	} else if(keycode == sys::virtual_key::E) {
		zoom_change = zoom * 0.9f;
	}
}

void display_data::on_key_up(sys::virtual_key keycode, sys::key_modifiers mod) {
	if(keycode == sys::virtual_key::LEFT) {
		pos_velocity.x = 0;
	} else if(keycode == sys::virtual_key::RIGHT) {
		pos_velocity.x = 0;
	} else if(keycode == sys::virtual_key::UP) {
		pos_velocity.y = 0;
	} else if(keycode == sys::virtual_key::DOWN) {
		pos_velocity.y = 0;
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
	screen_pos.x /= size.x / size.y;

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
	map_pos *= size;
	auto idx = int32_t(size.y - map_pos.y) * int32_t(size.x) + int32_t(map_pos.x);
	if(0 <= idx && size_t(idx) < province_id_map.size()) {
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
		auto fat_id = dcon::fatten(state.world, province::from_map_id(province_id_map[idx]));
		if(province_id_map[idx] < province::to_map_id(state.province_definitions.first_sea_province)) {
			set_selected_province(province_id_map[idx]);
		} else {
			set_selected_province(0);
		}
	} else {
		set_selected_province(0);
	}
}

int16_t display_data::get_selected_province() {
	return selected_province;
}

void display_data::set_selected_province(int16_t prov_id) {
	unhandled_province_selection = selected_province != prov_id;
	selected_province = prov_id;
}
}
