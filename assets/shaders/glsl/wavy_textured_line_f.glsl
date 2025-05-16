in float tex_coord;
in float o_dist;
in vec2 map_coord;
out vec4 frag_color;

uniform float gamma;
uniform vec2 screen_size;

uniform sampler2D provinces_texture_sampler;
uniform sampler2D province_fow;
uniform sampler2D line_texture;
uniform sampler2D provinces_sea_mask;

vec4 gamma_correct(vec4 colour) {
	return vec4(pow(colour.rgb, vec3(1.f / gamma)), colour.a);
}

void main() {
	// frag_color = vec4(tex_coord, tex_coord, tex_coord, 1.0);
	// return;

	vec4 out_color = texture(line_texture, vec2(o_dist, tex_coord));
	vec2 prov_id = texture(provinces_texture_sampler, gl_FragCoord.xy / screen_size).xy;
	if (texture(provinces_sea_mask, prov_id).x > 0 || (prov_id.x == 0.f && prov_id.y == 0.f)) {
        discard;
    }
	out_color.rgb *= texture(province_fow, prov_id).rgb;
	out_color.rgb *= out_color.a;

	float value = abs( (0.5f + sin(o_dist * 1.f) * 0.2) - tex_coord);


	if (value < 0.05f) {
		out_color.rgb = (vec3(value) + 1.f) * 0.5f;
	} else {
		out_color.rgb = vec3(1.f);
	}

	out_color.a = 1;

	frag_color = gamma_correct(out_color);
}
