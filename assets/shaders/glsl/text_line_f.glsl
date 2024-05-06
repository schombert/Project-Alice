in vec2 tex_coord;
in float type;
in float opacity;
layout (location = 0) out vec4 frag_color;

layout (binding = 0) uniform sampler2D texture0;
layout (binding = 1) uniform sampler2D texture1;
layout (binding = 2) uniform sampler2D texture2;
layout (binding = 3) uniform sampler2D texture3;

layout (location = 12) uniform float is_black;

layout (location = 11) uniform float gamma;
vec4 gamma_correct(vec4 colour) {
	return vec4(pow(colour.rgb, vec3(1.f / gamma)), colour.a);
}

void main() {
	float border_size = 0.055f;
	vec3 inner_color = vec3(1.0 - is_black, 1.0 - is_black, 1.0 - is_black);
	vec3 outer_color = vec3(1.0 * is_black, 1.0 * is_black, 1.0 * is_black);
	vec4 color_in = vec4(0, 0, 0, 0);

	if(type < 0.1f) {
		color_in = texture(texture0, tex_coord);
	} else if(type < 1.1f) {
		color_in = texture(texture1, tex_coord);
	} else if(type < 2.1f) {
		color_in = texture(texture2, tex_coord);
	} else if(type < 3.1f) {
		color_in = texture(texture3, tex_coord);
	}

	if(color_in.r > 0.505) {
		frag_color = vec4(inner_color, 1.0f);
	} else if(color_in.r > 0.50) {
		frag_color = vec4(mix(inner_color, outer_color,  1.0f - (color_in.r - 0.5f) * 200.0f), 1.0f);
	} else {
		float t = max(0.0f, color_in.r * 16.0f - 7.0f);
		frag_color = vec4(outer_color, t * t * t);
	}
	frag_color.a *= 0.8f;
    frag_color.a *= opacity;
	frag_color = gamma_correct(frag_color);
}
