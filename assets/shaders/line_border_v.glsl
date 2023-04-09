#version 430 core

// Goes from 0 to 1
layout (location = 0) in vec2 vertex_position;
layout (location = 1) in vec2 normal_direction;
layout (location = 2) in vec2 direction;

out vec2 tex_coord;
// Camera position
layout (location = 0) uniform vec2 offset;
layout (location = 1) uniform float aspect_ratio;
// Zoom: big numbers = close
layout (location = 2) uniform float zoom;
// The size of the map in pixels
layout (location = 3) uniform vec2 map_size;
// The scaling factor for the width
layout (location = 4) uniform float border_width;

// The borders are drawn by seperate quads.
// Each triangle in the quad is made up by two vertices on the same position and
// another one in the "direction" vector. Then all the vertices are offset in the "normal_direction".
void main() {
	float zoom_level = 1.5f * clamp(zoom, 2.f, 10.f) - 0.5f;
	float thickness = border_width / zoom_level;
	vec2 normal_vector = normalize(normal_direction) * thickness;
	// Extend the border slightly to make it fit together with any other border in any octagon direction.
	vec2 extend_vector = -normalize(direction) * thickness / (1 + sqrt(2));
	vec2 world_pos = vertex_position + vec2(-offset.x, offset.y);

	world_pos.x = mod(world_pos.x, 1.0f);

	world_pos.x *= map_size.x / map_size.y;
	world_pos += extend_vector + normal_vector;
	world_pos.x /= map_size.x / map_size.y;
	// Translates and scales the map coordinates to clip coordinates, i.e. -1 -> 1
	gl_Position = vec4(
		(2. * world_pos.x - 1.f) * zoom / aspect_ratio * map_size.x / map_size.y,
		(2. * world_pos.y - 1.f) * zoom,
		0.0, 1.0);
	tex_coord = vertex_position;
}
