in float tex_coord;
layout (location = 0) out vec4 frag_color;
layout (location = 11) uniform float gamma;

vec4 gamma_correct(vec4 colour) {
	return vec4(pow(colour.rgb, vec3(1.f / gamma)), colour.a);
}

void main() {
	vec4 out_color = vec4(1.f, 1.f, 1.f, 1.f);
	frag_color = gamma_correct(out_color);
}
