in vec2 tex_coord;
layout (location = 0) out vec4 frag_color;

layout (binding = 12) uniform sampler2D unit_arrow;

layout (location = 11) uniform float gamma;
vec4 gamma_correct(vec4 colour) {
	return vec4(pow(colour.rgb, vec3(1.f / gamma)), colour.a);
}

void main() {
	// frag_color = vec4(tex_coord.x, tex_coord.y, 0, 1.);
	vec4 OutColor = texture( unit_arrow, tex_coord );
	frag_color = gamma_correct(OutColor);
}
