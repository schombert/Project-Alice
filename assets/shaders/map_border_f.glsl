#version 430 core

in vec2 tex_coord;
layout (location = 0) out vec4 frag_color;

void main() {
	frag_color = vec4(1., 0., 0., 1.);
}
