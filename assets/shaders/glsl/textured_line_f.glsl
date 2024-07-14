in float tex_coord;
in float o_dist;
in vec2 map_coord;
out vec4 frag_color;

uniform float gamma;

uniform sampler2D line_texture;
uniform sampler2D colormap_water;
uniform sampler2D province_fow;
uniform sampler2D provinces_texture_sampler;

vec4 gamma_correct(vec4 colour) {
	return vec4(pow(colour.rgb, vec3(1.f / gamma)), colour.a);
}

void main() {
	vec4 out_color = texture(colormap_water, map_coord) * texture(line_texture, vec2(tex_coord, o_dist));
	frag_color = gamma_correct(out_color);
}
