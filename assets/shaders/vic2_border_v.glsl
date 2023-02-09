#version 430 core
layout (location = 0) in vec2 vertex_position;

out vec2 tex_coord;
layout (location = 0) uniform vec2 offset;
layout (location = 1) uniform float aspect_ratio;
layout (location = 2) uniform float zoom;
layout (location = 3) uniform vec2 map_size;

void main() {
	gl_Position = vec4(
		(2. * (-offset.x + vertex_position.x) - 1.f) * zoom / aspect_ratio * map_size.x / map_size.y,
		(2. * (+offset.y + vertex_position.y) - 1.f) * zoom,
		0.0, 1.0);
	tex_coord = vertex_position;
}
