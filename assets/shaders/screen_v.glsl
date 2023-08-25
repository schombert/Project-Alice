// The screen coordinates in normalized coordinates (0-1)
layout (location = 0) in vec2 vertex_position;

void main() {
	gl_Position = vec4(
		-1.0 + (2.0 * vertex_position.x),
		 1.0 - (2.0 * vertex_position.y),
		0.0, 1.0);
}
