in vec2 tex_coord;
layout (location = 0) out vec4 frag_color;

layout (binding = 15) uniform sampler2D national_border;
layout (location = 12) uniform float border_type;

layout (location = 11) uniform float gamma;
vec4 gamma_correct(vec4 colour) {
	return vec4(pow(colour.rgb, vec3(1.f / gamma)), colour.a);
}

void main() {
	vec4 out_color = texture(national_border, vec2(tex_coord.x, (tex_coord.y / 4.f) + (border_type * (1.f / 4.f))));
	//vec4 out_color = vec4(1.f, 0.f, 0.f, 1.f);
	frag_color = gamma_correct(out_color);
}
