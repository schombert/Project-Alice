layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texture_coordinate;

// Camera position
uniform vec2 offset;
uniform float aspect_ratio;

// Zoom: big numbers = close
uniform float zoom;

// The size of the map in pixels
uniform vec2 map_size;
uniform mat3 rotation;

// map projection
uniform uint subroutines_index;

out vec2 tex_coord;
out vec3 space_coords;

vec4 calc_gl_position(vec2 position) {
	return point_to_ogl_space(int(subroutines_index), position, map_size, rotation, offset, aspect_ratio, zoom);
}

void main()
{
	float angle_x = 2 * position.x * PI;
	float x = cos(angle_x);
	float y = sin(angle_x);
	float angle_y = position.y * PI;
	x *= sin(angle_y);
	y *= sin(angle_y);
	float z = cos(angle_y);
	space_coords = vec3(x, y, z);
	gl_Position = calc_gl_position(position);
	tex_coord = texture_coordinate;
}
