#version 430 core

in vec2 tex_coord;
layout (location = 0) out vec4 frag_color;
layout (binding = 0) uniform sampler2D provinces_texture_sampler;
layout (binding = 1) uniform sampler2D terrain_texture_sampler;
layout (binding = 2) uniform sampler2D rivers_texture_sampler;
layout (binding = 3) uniform sampler2DArray terrainsheet_texture_sampler;
layout (binding = 6) uniform sampler2D colormap_terrain;
layout (binding = 7) uniform sampler2D overlay;
layout (binding = 8) uniform sampler2D province_color;

// location 0 : offset
// location 1 : zoom
// location 2 : screen_size
layout (location = 3) uniform vec2 map_size;
layout (location = 4) uniform float time;

// sheet is composed of 64 files, in 4 cubes of 4 rows of 4 columns
// so each column has 8 tiles, and each row has 8 tiles too

float xx = 1 / map_size.x;
float yy = 1 / map_size.y;
vec2 pix = vec2(xx, yy);

vec4 get_terrain(vec2 tex_coords, vec2 corner, vec2 offset) {
	vec2 tex_coord_flipped = vec2(tex_coord.x, 1 - tex_coord.y);
	float index = texture(terrain_texture_sampler, tex_coord_flipped + 0.5 * pix * corner).r;
	index = floor(index * 256);
	float is_water = step(64, index);
	vec4 colour = texture(terrainsheet_texture_sampler, vec3(offset, index));
	return mix(colour, vec4(0.), is_water);
}

vec4 get_terrain_mix(vec2 tex_coords) {
	// Pixel size on map texture
	vec2 scaling = mod(tex_coord + 0.5 * pix, pix) / pix;

	vec2 offset = tex_coord / (16. * pix);

	vec4 colourlu = get_terrain(tex_coord, vec2(-1, -1), offset);
	vec4 colourld = get_terrain(tex_coord, vec2(-1, +1), offset);
	vec4 colourru = get_terrain(tex_coord, vec2(+1, -1), offset);
	vec4 colourrd = get_terrain(tex_coord, vec2(+1, +1), offset);

	vec4 colour_u = mix(colourlu, colourru, scaling.x);
	vec4 colour_d = mix(colourld, colourrd, scaling.x);
	return mix(colour_u, colour_d, 1-scaling.y);
}

void main() {
	vec4 terrain_background = texture(colormap_terrain, tex_coord);
	vec4 terrain = get_terrain_mix(tex_coord);
	frag_color = (terrain * 2. + terrain_background) / 3.;
}
