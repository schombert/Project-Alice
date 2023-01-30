#version 430 core

in vec2 tex_coord;
layout (location = 0) out vec4 frag_color;
layout (binding = 0) uniform sampler2D provinces_texture_sampler;
layout (binding = 1) uniform sampler2D terrain_texture_sampler;
layout (binding = 2) uniform sampler2D rivers_texture_sampler;
layout (binding = 3) uniform sampler2DArray terrainsheet_texture_sampler;

// sheet is composed of 64 files, in 4 cubes of 4 rows of 4 columns
// so each column has 8 tiles, and each row has 8 tiles too

const vec2 map_size = vec2(5616.0, 2160.0);

vec4 get_terrain(vec2 terrain_tex_coord, vec2 offset) {
	vec4 terrain = texture(terrain_texture_sampler, terrain_tex_coord);
	vec4 river = texture(rivers_texture_sampler, terrain_tex_coord);

	const int index = int(terrain.r * 64.0);
	vec4 color = texture(terrainsheet_texture_sampler, vec3(offset, index));
	return color;
}

vec4 get_terrain() {
	vec2 pix = vec2(1.0) / map_size;
	float xx = pix.x;
	float yy = pix.y;
	vec2 scaling = mod(tex_coord + 0.5 * pix, pix) / pix;

	vec2 offset = 196.0 * tex_coord;
	offset.y *= xx / yy;
	return get_terrain(tex_coord, offset);
}

void main() {
	vec4 terrain_color = get_terrain();
	vec4 province_color = texture(provinces_texture_sampler, tex_coord);
	vec4 final_color = mix(province_color, terrain_color, 0.5);
	frag_color = final_color;
}
