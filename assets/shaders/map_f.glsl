#version 430 core

in vec2 tex_coord;
layout (location = 0) out vec4 frag_color;
layout (binding = 0) uniform sampler2D provinces_texture_sampler;
layout (binding = 1) uniform sampler2D terrain_texture_sampler;
layout (binding = 3) uniform sampler2DArray terrainsheet_texture_sampler;
layout (binding = 6) uniform sampler2D colormap_terrain;

// location 0 : offset
// location 1 : zoom
// location 2 : screen_size
layout (location = 3) uniform vec2 map_size;

float xx = 1 / map_size.x;
float yy = 1 / map_size.y;
vec2 pix = vec2(xx, yy);

// The terrain color from the current texture coordinate offset with one pixel in the "corner" direction
vec4 get_terrain(vec2 corner, vec2 offset) {
	float index = texture(terrain_texture_sampler, tex_coord + 0.5 * pix * corner).r;
	index = floor(index * 256);
	float is_water = step(64, index);
	vec4 colour = texture(terrainsheet_texture_sampler, vec3(offset, index));
	return mix(colour, vec4(0.), is_water);
}

vec4 get_terrain_mix() {
	// Pixel size on map texture
	vec2 scaling = mod(tex_coord + 0.5 * pix, pix) / pix;

	vec2 offset = tex_coord / (16. * pix);

	vec4 colourlu = get_terrain(vec2(-1, -1), offset);
	vec4 colourld = get_terrain(vec2(-1, +1), offset);
	vec4 colourru = get_terrain(vec2(+1, -1), offset);
	vec4 colourrd = get_terrain(vec2(+1, +1), offset);

	// Mix together the terrains based on close they are to the current texture coordinate
	vec4 colour_u = mix(colourlu, colourru, scaling.x);
	vec4 colour_d = mix(colourld, colourrd, scaling.x);
	return mix(colour_u, colour_d, scaling.y);
}

// The terrain map
// No province color is used here
void main() {
	vec4 terrain_background = texture(colormap_terrain, tex_coord);
	vec4 terrain = get_terrain_mix();
	// Mixes the terrains from "texturesheet.tga" with the "colormap.dds" background color.
	// frag_color = (terrain * 2. + terrain_background) / 3.;
	frag_color = terrain;
}
