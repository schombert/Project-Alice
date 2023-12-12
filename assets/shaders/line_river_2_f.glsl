in vec2 tex_coord;
in vec2 map_coord;
in float tex_type;

layout (location = 0) out vec4 frag_color;

layout (binding = 0) uniform sampler2D provinces_texture_sampler;
layout (binding = 5) uniform sampler2D colormap_water;
layout (binding = 13) uniform sampler2D province_fow;
layout (binding = 14) uniform sampler2D river_body;

layout (location = 11) uniform float gamma;
layout (location = 12) uniform float time;
layout (location = 13) uniform float pass;
vec4 gamma_correct(vec4 colour) {
	return vec4(pow(colour.rgb, vec3(1.f / gamma)), colour.a);
}

void main() {
	vec4 out_color = texture(colormap_water, map_coord);
	vec4 graymap = texture(river_body, vec2(tex_coord.y, tex_coord.x - time));
	out_color.rgb *= vec3(graymap.r, graymap.r, graymap.r);
	out_color.a = graymap.a;
	vec2 prov_id = texture(provinces_texture_sampler, map_coord).xy;
	out_color.rgb *= texture(province_fow, prov_id).r;
	frag_color = gamma_correct(out_color);
}
