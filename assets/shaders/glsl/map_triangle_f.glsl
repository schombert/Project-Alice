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
	if (length(tex_coord) > 1.f) {
		discard;
	}

	vec4 out_color = texture(texture_sampler, tex_coord / 10.f);
	
	vec3 inner_color = vec3(0.6f, 0.2, 0.2f);

	float darkness = max(0.f, -dot(light_direction, space_coords) + 0.2f);

	float light = 0.f;

	if (ignore_light == 0.f) {
		light = smootherstep(1.f - length(tex_coord)) * darkness;
		//inner_color *= 5.f * out_color.a * out_color.rgb * darkness * ;
	}

	if (length(tex_coord) > 0.5f) {
		if (ignore_light != 0.f) {
			discard;
		}
		frag_color = vec4(light, light, light, 0.f);
	} else if (length(tex_coord) > 0.25f) {
		frag_color = vec4(light, light, light, texture(texture_sampler, tex_coord));
	} else {
		frag_color = vec4(inner_color, texture(texture_sampler, tex_coord));
	}

	frag_color = gamma_correct(frag_color) + vec4(inner_color, 0.f) * light;
}
