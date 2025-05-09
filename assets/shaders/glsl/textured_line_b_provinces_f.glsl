in float tex_coord;
in float o_dist;
in vec2 map_coord;
in float distance_to_national_border;
flat in vec2 frag_province_index;
layout(location = 0) out vec4 frag_color;

void main() {
	frag_color = vec4(frag_province_index, distance_to_national_border, 1.f);
}
