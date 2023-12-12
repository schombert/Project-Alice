in vec2 tex_coord;
in vec2 map_coord;
in float tex_type;

layout (location = 0) out vec4 frag_color;
layout (binding = 5) uniform sampler2D colormap_water;

layout (location = 11) uniform float gamma;
vec4 gamma_correct(vec4 colour) {
	return vec4(pow(colour.rgb, vec3(1.f / gamma)), colour.a);
}

void main() {
	vec4 out_color = texture(colormap_water, map_coord);
	frag_color = gamma_correct(out_color);
}
