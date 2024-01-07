in float tex_coord;
in float o_dist;
in vec2 map_coord;

layout (location = 0) out vec4 frag_color;

layout (binding = 0) uniform sampler2D provinces_texture_sampler;
layout (binding = 13) uniform sampler2D province_fow;
layout (binding = 14) uniform sampler2D line_texture;
layout (location = 11) uniform float gamma;

vec4 gamma_correct(vec4 colour) {
	return vec4(pow(colour.rgb, vec3(1.f / gamma)), colour.a);
}

void main() {
	vec4 out_color = texture(line_texture, vec2(o_dist, tex_coord));
	vec2 prov_id = texture(provinces_texture_sampler, map_coord).xy;
	out_color.rgb *= texture(province_fow, prov_id).rgb;
	frag_color = gamma_correct(out_color);
}
