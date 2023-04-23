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
layout (location = 5) uniform mat3 rotation;

void main() {
	vec2 world_pos = vertex_position + vec2(-offset.x, offset.y);
	world_pos.x = mod(world_pos.x, 1.0f);
	vec3 new_world_pos;
	vec2 angle;
	float section = map_size.x / 256;
	float angle_x1 = 2 * 3.1415 * floor(vertex_position.x * section) / section;
	float angle_x2 = 2 * 3.1415 * floor(vertex_position.x * section + 1) / section;
	new_world_pos.x = mix(cos(angle_x1), cos(angle_x2), mod(vertex_position.x * section, 1));
	new_world_pos.y = mix(sin(angle_x1), sin(angle_x2), mod(vertex_position.x * section, 1));
	angle.y = vertex_position.y * 3.1415;
	new_world_pos.x *= sin(angle.y);
	new_world_pos.y *= sin(angle.y);
	new_world_pos.z = cos(angle.y);
	new_world_pos = rotation * new_world_pos;
	new_world_pos *= 0.2;
	new_world_pos.xz *= -1;
	new_world_pos.xz += 0.5;

	// Translates and scales the map coordinates to clip coordinates, i.e. -1 -> 1
	gl_Position = vec4(
		(2. * new_world_pos.x - 1.f) / aspect_ratio  * zoom,
		(2. * new_world_pos.z - 1.f) * zoom,
		(2. * new_world_pos.y - 1.f), 1.0);
	tex_coord = vertex_position;
}
