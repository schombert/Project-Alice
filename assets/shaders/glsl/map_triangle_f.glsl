in vec2 tex_coord;
in vec3 space_coords;
out vec4 frag_color;
uniform vec3 light_direction;
uniform float ignore_light;
uniform float gamma;
uniform sampler2D texture_sampler;

vec4 gamma_correct(vec4 colour) {
	return vec4(pow(colour.rgb, vec3(1.f / gamma)), colour.a);
}

void main() {

	vec4 out_color = texture(texture_sampler, tex_coord);
	if (ignore_light == 0.f) {
		float darkness = max(0.f, -dot(light_direction, space_coords) + 0.2f);
		out_color.rgb *= 5.f * out_color.a * out_color.rgb * darkness;
	}
	frag_color = gamma_correct(out_color);
}
