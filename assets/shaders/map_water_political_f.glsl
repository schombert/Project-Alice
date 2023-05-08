#version 430 core

in vec2 tex_coord;
layout (location = 0) out vec4 frag_color;
layout (binding = 5) uniform sampler2D colormap_water;
layout (binding = 7) uniform sampler2D overlay;

// location 0 : offset
// location 1 : zoom
// location 2 : screen_size
layout (location = 3) uniform vec2 map_size;

// Blue color mixed with the map overlay texture.
void main() {
	//vec4 water_background = texture(colormap_water, tex_coord);
	vec3 water_background = vec3(0.21, 0.38, 0.55);
	vec3 color = water_background.rgb;

	// The "foldable map" overlay effect
	vec4 OverlayColor = texture(overlay, tex_coord * vec2(11., 11.*map_size.y/map_size.x));
	vec4 OutColor;
	OutColor.r = OverlayColor.r < .5 ? (2. * OverlayColor.r * color.r) : (1. - 2. * (1. - OverlayColor.r) * (1. - color.r));
	OutColor.g = OverlayColor.r < .5 ? (2. * OverlayColor.g * color.g) : (1. - 2. * (1. - OverlayColor.g) * (1. - color.g));
	OutColor.b = OverlayColor.b < .5 ? (2. * OverlayColor.b * color.b) : (1. - 2. * (1. - OverlayColor.b) * (1. - color.b));
	OutColor.a = OverlayColor.a;

	frag_color = OutColor;
}
