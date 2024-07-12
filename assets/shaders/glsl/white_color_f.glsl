out vec4 frag_color;
uniform float gamma;
vec4 gamma_correct(vec4 colour) {
	return vec4(pow(colour.rgb, vec3(1.f / gamma)), colour.a);
}

void main() {
	frag_color = gamma_correct(vec4(0.85f, 0.85f, 0.85f, 1.f));
}
