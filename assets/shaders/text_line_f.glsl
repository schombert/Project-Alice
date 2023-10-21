in vec2 tex_coord;
layout (location = 0) out vec4 frag_color;

layout (binding = 14) uniform sampler2D font_atlas;

void main() {
	vec4 OutColor = texture( font_atlas, tex_coord );
	frag_color = OutColor;
}
