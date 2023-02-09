#version 430 core
layout (location = 0) in vec2 vertex_position;
layout (location = 1) in vec2 direction;

out vec2 tex_coord;
layout (location = 0) uniform vec2 offset;
layout (location = 1) uniform float aspect_ratio;
layout (location = 2) uniform float zoom;
layout (location = 3) uniform vec2 map_size;

void main() {
	vec2 thickness = normalize(direction) * 0.005 / zoom;
	thickness.x *= map_size.y / map_size.x;
	gl_Position = vec4(
		(2. * (-offset.x + vertex_position.x + thickness.x) - 1.f) * zoom / aspect_ratio * map_size.x / map_size.y,
		(2. * (+offset.y + vertex_position.y + thickness.y) - 1.f) * zoom,
		0.0, 1.0);
	tex_coord = vertex_position;
}
