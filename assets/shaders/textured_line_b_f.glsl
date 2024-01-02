in float tex_coord;
in float o_dist;
in vec2 map_coord;

layout (location = 0) out vec4 frag_color;

layout (binding = 14) uniform sampler2D line_texture;
layout (location = 11) uniform float gamma;

vec4 gamma_correct(vec4 colour) {
	return vec4(pow(colour.rgb, vec3(1.f / gamma)), colour.a);
}

void main() {
	vec4 out_color = texture(line_texture, vec2(o_dist, tex_coord));
	frag_color = gamma_correct(out_color);
}
