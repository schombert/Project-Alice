layout (location = 0) in vec2 position;

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

void main() {
	gl_Position = calc_gl_position(position);
	space_coords =  point_to_sphere(position);
	tex_coord = position;
}
