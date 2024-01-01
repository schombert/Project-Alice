in vec2 tex_coord;
layout (location = 0) out vec4 frag_color;
layout (binding = 14) uniform sampler2D diffuse_texture;

layout (location = 11) uniform float gamma;

vec4 gamma_correct(vec4 colour) {
	return vec4(pow(colour.rgb, vec3(1.f / gamma)), colour.a);
}

void main() {
	vec4 out_color = texture(diffuse_texture, tex_coord);
	//out_color.a = 0.f;
	frag_color = gamma_correct(out_color);
}
