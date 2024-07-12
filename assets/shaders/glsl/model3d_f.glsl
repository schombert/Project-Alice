in vec2 tex_coord;
out vec4 frag_color;

uniform float gamma;

uniform sampler2D diffuse_texture;

vec4 gamma_correct(vec4 colour) {
	return vec4(pow(colour.rgb, vec3(1.f / gamma)), colour.a);
}

void main() {
	vec4 out_color = texture(diffuse_texture, tex_coord);
	//out_color.a = (out_color.r * out_color.g * out_color.b == 0.f) ? 0.f : out_color.a;
	frag_color = gamma_correct(out_color);
}
