#version 430 core
// Goes from 0 to 1
layout (location = 0) in vec2 vertex_position;
// layout (location = 1) in vec2 v_tex_coord;

out vec2 tex_coord;
// Camera position
layout (location = 0) uniform vec2 offset;
layout (location = 1) uniform float aspect_ratio;
// Zoom: big numbers = close
layout (location = 2) uniform float zoom;
// The size of the map in pixels
layout (location = 3) uniform vec2 map_size;

void main() {
	vec2 world_pos = vertex_position + vec2(-offset.x, offset.y);
	world_pos.x = mod(world_pos.x, 1.0f);

	// Translates and scales the map coordinates to clip coordinates, i.e. -1 -> 1
	gl_Position = vec4(
		(2. * world_pos.x - 1.f) * zoom / aspect_ratio * map_size.x / map_size.y,
		(2. * world_pos.y - 1.f) * zoom,
		0.0, 1.0);
	tex_coord = vertex_position;
}
