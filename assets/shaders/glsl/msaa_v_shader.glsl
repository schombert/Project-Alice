layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tcoord;
out vec2 texcoord;
void main() {
	texcoord = tcoord;
	gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);
}
