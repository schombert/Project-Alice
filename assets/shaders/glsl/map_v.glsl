// Goes from 0 to 1
layout (location = 0) in vec2 vertex_position;
out vec2 tex_coord;
out vec3 space_coords;

// Camera position
uniform vec2 offset;
uniform float aspect_ratio;
// Zoom: big numbers = close
uniform float zoom;
// The size of the map in pixels
uniform vec2 map_size;
uniform mat3 rotation;
uniform uint subroutines_index;

vec4 calc_gl_position(vec2 position) {
	return point_to_ogl_space(int(subroutines_index), position, map_size, rotation, offset, aspect_ratio, zoom);
}

void main() {
	gl_Position = calc_gl_position(vertex_position);
	space_coords =  point_to_sphere(vertex_position);
	tex_coord = vertex_position;
}
