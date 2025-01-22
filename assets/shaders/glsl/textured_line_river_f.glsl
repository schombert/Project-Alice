in float tex_coord;
in float o_dist;
in vec2 map_coord;
out vec4 frag_color;

uniform float gamma;

uniform vec2 screen_size;

uniform sampler2D line_texture;
uniform sampler2D colormap_water;
uniform sampler2D province_fow;
uniform sampler2D provinces_texture_sampler;
uniform sampler2D provinces_sea_mask;

vec4 gamma_correct(vec4 colour) {
	return vec4(pow(colour.rgb, vec3(1.f / gamma)), colour.a);
}

void main() {
	vec4 out_color = texture(colormap_water, map_coord) * texture(line_texture, vec2(tex_coord, o_dist));
    vec2 prov_id = texture(provinces_texture_sampler, gl_FragCoord.xy / screen_size).xy;
    if (texture(provinces_sea_mask, prov_id).x > 0 || (prov_id.x == 0.f && prov_id.y == 0.f)) {
        discard;
    }
    float is_sea = texture(provinces_sea_mask, prov_id).x * 1000.f;
	frag_color = gamma_correct(out_color);
    //frag_color = vec4(0, o_dist, 0.f, 1.f);
}
