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

vec4 get_terrain(vec2 tex_coord)
{
	// Pixel size on map texture
	float xx = 1 / map_size.x;
	float yy = 1 / map_size.y;
	vec2 pix = vec2(xx, yy);
	vec2 scaling = mod(tex_coord + 0.5 * pix, pix) / pix;
	
	vec2 offset = 196.0 * tex_coord;
	offset.y *= xx / yy;

	vec2 tex_coord_flipped = vec2(tex_coord.x, 1 - tex_coord.y);
	vec4 colour0 = texture(terrain_texture_sampler, tex_coord_flipped + 0.5 * vec2(-xx, -yy));
	vec4 colour1 = texture(terrain_texture_sampler, tex_coord_flipped + 0.5 * vec2(-xx, yy));
	vec4 colour2 = texture(terrain_texture_sampler, tex_coord_flipped + 0.5 * vec2(xx, -yy));
	vec4 colour3 = texture(terrain_texture_sampler, tex_coord_flipped + 0.5 * vec2(xx, yy));

	const int index0 = int(colour0.r * 256);
	const int index1 = int(colour1.r * 256);
	const int index2 = int(colour2.r * 256);
	const int index3 = int(colour3.r * 256);

	vec4 emptyCol = vec4(0., 0., 0., 0.);
	vec4 colourlu = texture(terrainsheet_texture_sampler, vec3(offset, index0));
	vec4 colourld = texture(terrainsheet_texture_sampler, vec3(offset, index1));
	vec4 colourru = texture(terrainsheet_texture_sampler, vec3(offset, index2));
	vec4 colourrd = texture(terrainsheet_texture_sampler, vec3(offset, index3));

	colour0 = mix(colourlu, colourru, scaling.x);
	colour1 = mix(colourld, colourrd, scaling.x);
	return mix(colour0, colour1, 1-scaling.y);
}

void main() {
	vec4 terrain_color = get_terrain(tex_coord);
	vec4 province_color = texture(provinces_texture_sampler, tex_coord);
	vec4 final_color = mix(province_color, terrain_color, 0.5);
	frag_color = terrain_color;
}
