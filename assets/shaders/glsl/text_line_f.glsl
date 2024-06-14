in vec3 tex_coord;
in float opacity;
in float text_size;
layout (binding = 0) uniform sampler2D texture_sampler;
layout (location = 0) out vec4 frag_color;
layout (location = 12) uniform float is_black;
layout (location = 11) uniform float gamma;
vec4 gamma_correct(vec4 colour) {
	return vec4(pow(colour.rgb, vec3(1.f / gamma)), colour.a);
}

void main() {
	float border_size = 0.022f;
	vec3 inner_color = vec3(1.0 - is_black, 1.0 - is_black, 1.0 - is_black);
	vec3 outer_color = vec3(1.0 * is_black, 1.0 * is_black, 1.0 * is_black);
    
    outer_color = mix(inner_color, outer_color, text_size * 40.f);
	
	vec4 color_in = texture(texture_sampler, vec2(tex_coord.rg));
	if(color_in.r > 0.505) {
		frag_color = vec4(inner_color, 1.0f);
	} else if(color_in.r > 0.50) {
		frag_color = vec4(mix(inner_color, outer_color,  1.0f - (color_in.r - 0.5f) * 200.0f), 1.0f);
	} else {
		float t = max(0.0f, color_in.r * 16.0f - 7.0f);
		frag_color = vec4(outer_color, pow(t, 5.f));
	}

	frag_color.a *= opacity;
	frag_color = gamma_correct(frag_color);
}
