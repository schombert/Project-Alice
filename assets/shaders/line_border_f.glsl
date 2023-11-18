in vec2 tex_coord;
layout (location = 0) out vec4 frag_color;

layout (location = 11) uniform float gamma;
vec4 gamma_correct(vec4 colour) {
	return vec4(pow(colour.rgb, vec3(1.f / gamma)), colour.a);
}

void main() {
	frag_color = gamma_correct(vec4(0., 0., 0., 1.));
}
