in vec2 tex_coord;
out vec4 frag_color;
uniform float gamma;
uniform sampler2D texture_sampler;

vec4 gamma_correct(vec4 colour) {
	return vec4(pow(colour.rgb, vec3(1.f / gamma)), colour.a);
}

void main() {
	vec4 out_color = texture(texture_sampler, tex_coord);
	out_color.rgb *= out_color.a;
	frag_color = gamma_correct(out_color);
}
