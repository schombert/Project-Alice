in float tex_coord;
in float o_dist;
in vec2 map_coord;
out vec4 frag_color;

uniform float gamma;

uniform sampler2D provinces_texture_sampler;
uniform sampler2D province_fow;
uniform sampler2D line_texture;

vec4 gamma_correct(vec4 colour) {
	return vec4(pow(colour.rgb, vec3(1.f / gamma)), colour.a);
}

void main() {
	// frag_color = vec4(tex_coord, tex_coord, tex_coord, 1.0);
	// return;


	vec4 out_color = texture(line_texture, vec2(o_dist, tex_coord));
	vec2 prov_id = texture(provinces_texture_sampler, map_coord).xy;
	out_color.rgb *= texture(province_fow, prov_id).rgb;
	frag_color = gamma_correct(out_color);
}
