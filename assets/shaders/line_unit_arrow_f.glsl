in vec2 tex_coord;
layout (location = 0) out vec4 frag_color;

layout (binding = 12) uniform sampler2D unit_arrow;

void main() {
	// frag_color = vec4(tex_coord.x, tex_coord.y, 0, 1.);
	vec4 OutColor = texture( unit_arrow, tex_coord );
	frag_color = OutColor;
}
