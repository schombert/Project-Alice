#version 430 core

in vec2 tex_coord;
layout (location = 0) out vec4 frag_color;
layout (binding = 0) uniform sampler2D provinces_texture_sampler;
layout (binding = 7) uniform sampler2D overlay;
layout (binding = 8) uniform sampler2D province_color;
layout (binding = 9) uniform sampler2D colormap_political;
layout (binding = 10) uniform sampler2D province_highlight;

// location 0 : offset
// location 1 : zoom
// location 2 : screen_size
layout (location = 3) uniform vec2 map_size;

void main() {
	vec2 prov_id = texture(provinces_texture_sampler, tex_coord).xy;
	vec3 political = clamp(texture(province_color, prov_id) + texture(province_highlight, prov_id), 0.0, 1.0).rgb;
	political = political - 0.7;

	vec4 OverlayColor = texture(overlay, tex_coord * vec2(11., 11.*map_size.y/map_size.x));
	vec3 background = texture(colormap_political, tex_coord).rgb;

	vec4 OutColor;
	OutColor.r = OverlayColor.r < .5 ? (2. * OverlayColor.r * political.r) : (1. - 2. * (1. - OverlayColor.r) * (1. - political.r));
	OutColor.g = OverlayColor.r < .5 ? (2. * OverlayColor.g * political.g) : (1. - 2. * (1. - OverlayColor.g) * (1. - political.g));
	OutColor.b = OverlayColor.b < .5 ? (2. * OverlayColor.b * political.b) : (1. - 2. * (1. - OverlayColor.b) * (1. - political.b));
	OutColor.a = OverlayColor.a;

	OutColor.rgb = mix(background, OutColor.rgb, 0.3);

	OutColor.rgb *= 1.5;

	frag_color = OutColor;
}
