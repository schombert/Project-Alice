#version 430 core
layout (location = 0) in vec2 vertex_position;
layout (location = 1) in vec2 v_tex_coord;
		
out vec2 tex_coord;
layout (location = 0) uniform vec2 offset;
layout (location = 1) uniform vec2 map_size;
layout (location = 2) uniform float zoom;

void main() {
	gl_Position = vec4(
		(((offset.x + vertex_position.x) * map_size.x) - (map_size.x / 2.f)) * zoom,
		(((offset.y + vertex_position.y) * map_size.y) - (map_size.y / 2.f)) * zoom,
		0.0, 1.0);
	tex_coord = v_tex_coord;
}
