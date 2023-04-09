#version 430 core

in vec2 tex_coord;
layout (location = 0) out vec4 frag_color;
layout (binding = 0) uniform sampler2D provinces_texture_sampler;
layout (binding = 7) uniform sampler2D overlay;
layout (binding = 8) uniform sampler2DArray province_color;
layout (binding = 9) uniform sampler2D colormap_political;
layout (binding = 10) uniform sampler2D province_highlight;
layout (binding = 11) uniform sampler2D stripes_texture;

// location 0 : offset
// location 1 : zoom
// location 2 : screen_size
layout (location = 3) uniform vec2 map_size;

// The water map
// Map modes with the map overlay texture. No terrain.
void main() {
	vec2 prov_id = texture(provinces_texture_sampler, tex_coord).xy;

	// The primary and secondary map mode province colors
	vec4 prov_color = texture(province_color, vec3(prov_id, 0.));
	vec4 stripe_color = texture(province_color, vec3(prov_id, 1.));

	vec2 stripe_coord = tex_coord * vec2(512., 512. * map_size.y / map_size.x);

	// Mix together the primary and secondary colors with the stripes
	float stripeFactor = texture(stripes_texture, stripe_coord).a;
	vec3 political = clamp(mix(prov_color, stripe_color, stripeFactor) + texture(province_highlight, prov_id), 0.0, 1.0).rgb;
	political = political - 0.7;

	// The "foldable map" overlay effect
	vec4 OverlayColor = texture(overlay, tex_coord * vec2(11., 11.*map_size.y/map_size.x));
	vec4 OutColor;
	OutColor.r = OverlayColor.r < .5 ? (2. * OverlayColor.r * political.r) : (1. - 2. * (1. - OverlayColor.r) * (1. - political.r));
	OutColor.g = OverlayColor.r < .5 ? (2. * OverlayColor.g * political.g) : (1. - 2. * (1. - OverlayColor.g) * (1. - political.g));
	OutColor.b = OverlayColor.b < .5 ? (2. * OverlayColor.b * political.b) : (1. - 2. * (1. - OverlayColor.b) * (1. - political.b));
	OutColor.a = OverlayColor.a;

	vec3 background = texture(colormap_political, tex_coord).rgb;
	OutColor.rgb = mix(background, OutColor.rgb, 0.3);

	OutColor.rgb *= 1.5;

	frag_color = OutColor;
}
