out vec4 out_color;
in vec2 texcoord;
layout (binding = 0) uniform sampler2D screen_texture;
void main() {
	out_color = texture(screen_texture, texcoord);
}
