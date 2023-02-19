#version 430 core
layout (location = 0) in vec2 vertex_position;
layout (location = 1) in vec2 v_tex_coord;
layout (location = 2) in vec2 border_offset;

out vec2 tex_coord;
out vec2 type;
layout (location = 0) uniform vec2 offset;
layout (location = 1) uniform float aspect_ratio;
layout (location = 2) uniform float zoom;
layout (location = 3) uniform vec2 map_size;

#define BORDER_PADDING_OFFSET 0.02f;

void main() {
	gl_Position = vec4(
		(2. * (-offset.x + vertex_position.x) - 1.f) * zoom / aspect_ratio * map_size.x / map_size.y,
		(2. * (+offset.y + vertex_position.y) - 1.f) * zoom,
		0.0, 1.0);

	tex_coord = v_tex_coord;
	tex_coord.y = 1 - tex_coord.y;
	tex_coord.x *= 1.0f - 2 * BORDER_PADDING_OFFSET;
	tex_coord.x += BORDER_PADDING_OFFSET;
	tex_coord.x *= 0.8 / 32;
	tex_coord.y *= 0.25f - 2 * BORDER_PADDING_OFFSET;
	tex_coord.y += BORDER_PADDING_OFFSET;

	type = border_offset;
}
