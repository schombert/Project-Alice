layout (location = 0) in vec2 pos; //0
layout (location = 1) in vec2 tcoord; //1
out vec2 texcoord;

void main() {
	texcoord = tcoord;
	gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);
}
