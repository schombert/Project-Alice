#version 430 core
layout (location = 0) in vec2 vertex_position;
// layout (location = 1) in vec2 v_tex_coord;

out vec2 tex_coord;
layout (location = 0) uniform vec2 offset;
layout (location = 1) uniform float aspect_ratio;
layout (location = 2) uniform float zoom;
layout (location = 3) uniform vec2 map_size;

void main() {
	vec2 world_pos = vertex_position + vec2(-offset.x, offset.y);
	world_pos.x *= map_size.x / map_size.y;
	gl_Position = vec4(
		(2. * world_pos.x - 1.f) * zoom / aspect_ratio,
		(2. * world_pos.y - 1.f) * zoom,
		0.0, 1.0);
	tex_coord = vertex_position;
}
