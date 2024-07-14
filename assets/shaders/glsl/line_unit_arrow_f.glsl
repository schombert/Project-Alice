in vec2 tex_coord;
out vec4 frag_color;

uniform float gamma;

uniform sampler2D unit_arrow;

vec4 gamma_correct(vec4 colour) {
	return vec4(pow(colour.rgb, vec3(1.f / gamma)), colour.a);
}

void main() {
	vec4 OutColor = texture( unit_arrow, tex_coord );
	frag_color = gamma_correct(OutColor);
}
