layout (location = 0) in vec2 position;

// Sprite position
uniform vec2 position_offset;
uniform vec2 scale;

// Texture params
uniform vec2 texture_start;
uniform vec2 texture_size;

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

vec4 calc_gl_position(vec2 position) {
	return point_to_ogl_space(int(subroutines_index), position, map_size, rotation, offset, aspect_ratio, zoom);
}

void main()
{
	gl_Position = calc_gl_position(position * scale + position_offset);

	tex_coord = texture_start + ((position + vec2(1.f, 1.f)) / vec2(2.f, 2.f)) * texture_size;
}
