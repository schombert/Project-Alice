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
layout (location = 5) uniform mat3 rotation;

#define PI 3.1415926538

subroutine vec4 calc_gl_position_class(vec2 world_pos);
layout(location = 0) subroutine uniform calc_gl_position_class calc_gl_position;

layout(index = 0) subroutine(calc_gl_position_class)
vec4 globe_coords(vec2 world_pos) {

	vec3 new_world_pos;
	float section = map_size.x / 256;
	float angle_x1 = 2 * PI * floor(world_pos.x * section) / section;
	float angle_x2 = 2 * PI * floor(world_pos.x * section + 1) / section;
	new_world_pos.x = mix(cos(angle_x1), cos(angle_x2), mod(world_pos.x * section, 1));
	new_world_pos.y = mix(sin(angle_x1), sin(angle_x2), mod(world_pos.x * section, 1));
	float angle_y = world_pos.y * PI;
	new_world_pos.x *= sin(angle_y);
	new_world_pos.y *= sin(angle_y);
	new_world_pos.z = cos(angle_y);
	new_world_pos = rotation * new_world_pos;
	new_world_pos *= 0.2;
	new_world_pos.xz *= -1;
	new_world_pos.xyz += 0.5;

	return vec4(
		(2. * new_world_pos.x - 1.f) / aspect_ratio  * zoom,
		(2. * new_world_pos.z - 1.f) * zoom,
		(2. * new_world_pos.y - 1.f), 1.0);
}

layout(index = 1) subroutine(calc_gl_position_class)
vec4 flat_coords(vec2 world_pos) {
	world_pos += vec2(-offset.x, offset.y);
	world_pos.x = mod(world_pos.x, 1.0f);
	return vec4(
		(2. * world_pos.x - 1.f) * zoom / aspect_ratio * map_size.x / map_size.y,
		(2. * world_pos.y - 1.f) * zoom,
		0.0, 1.0);
}

// The borders are drawn by seperate quads.
// Each triangle in the quad is made up by two vertices on the same position and
// another one in the "direction" vector. Then all the vertices are offset in the "normal_direction".
void main() {
	float zoom_level = 1.5f * clamp(zoom, 2.f, 10.f) - 0.5f;
	float thickness = border_width / zoom_level;
	vec2 normal_vector = normalize(normal_direction) * thickness;
	// Extend the border slightly to make it fit together with any other border in any octagon direction.
	vec2 extend_vector = -normalize(direction) * thickness / (1 + sqrt(2));
	vec2 world_pos = vertex_position;

	
	world_pos.x *= map_size.x / map_size.y;
	world_pos += extend_vector + normal_vector;
	world_pos.x /= map_size.x / map_size.y;

	gl_Position = calc_gl_position(world_pos);
	tex_coord = vertex_position;
}
