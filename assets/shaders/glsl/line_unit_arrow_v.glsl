// Goes from 0 to 1
layout (location = 0) in vec2 vertex_position;
layout (location = 1) in vec2 normal_direction;
layout (location = 2) in vec2 direction;
layout (location = 3) in vec2 texture_coord;
layout (location = 4) in float type;

out vec2 tex_coord;
// Camera position
uniform vec2 offset;
uniform float aspect_ratio;
// Zoom: big numbers = close
uniform float zoom;
// The size of the map in pixels
uniform vec2 map_size;
// The scaling factor for the width
uniform float border_width;
uniform mat3 rotation;
uniform uint subroutines_index;

vec4 calc_gl_position(vec2 position) {
	return point_to_ogl_space(int(subroutines_index), position, map_size, rotation, offset, aspect_ratio, zoom);
}

// The borders are drawn by seperate quads.
// Each triangle in the quad is made up by two vertices on the same position and
// another one in the "direction" vector. Then all the vertices are offset in the "normal_direction".
void main() {
	float thickness = border_width * 0.25f;
	vec2 rot_direction = vec2(-direction.y, direction.x);
	vec2 normal_vector = normalize(normal_direction) * thickness;
	// Extend the border slightly to make it fit together with any other border in any octagon direction.
	vec2 extend_vector = -normalize(direction) * thickness;
	extend_vector *= round(type) == 3.f ? 2.f : 0.f;
	vec2 world_pos = vertex_position;

	world_pos.x *= map_size.x / map_size.y;
	world_pos += normal_vector;
	world_pos += extend_vector;
	world_pos.x /= map_size.x / map_size.y;

	gl_Position = calc_gl_position(world_pos);
	tex_coord = texture_coord / 2.f + vec2(0.5 * mod(round(type), 2.f), 0.5 * floor(mod(type, 3.0f) / 2.f));
}
