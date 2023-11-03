out vec4 out_color;
in vec2 texcoord;
layout (binding = 0) uniform sampler2D screen_texture;
void main() {
	vec3 col = texture(screen_texture, texcoord).rgb;
	float grayscale = 0.2126 * col.r + 0.7152 * col.g + 0.0722 * col.b;
	out_color = vec4(vec3(grayscale), 1.0);
}