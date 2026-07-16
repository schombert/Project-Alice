// Goes from 0 to 1
layout (location = 0) in vec2 vertex_position;
layout (location = 1) in vec2 corner_direction;
layout (location = 2) in vec2 direction;
layout (location = 3) in vec2 vertexUV;
layout (location = 4) in float thickness;
layout (location = 5) in int vertexIndex;

out vec2 uv;
flat out int bufferIndex;
flat out float opacity;

uniform vec2 offset;
uniform float aspect_ratio;
uniform float zoom;
uniform vec2 map_size;
uniform mat3 rotation;
uniform float time;
uniform uint subroutines_index;

vec4 calc_gl_position(vec2 position) {
	return point_to_ogl_space(int(subroutines_index), position, map_size, rotation, offset, aspect_ratio, zoom);
}


void main() {
	vec2 normal_dir = rotate_left(int(subroutines_index), vertex_position, direction, map_size);
	vec4 center_point = calc_gl_position(vertex_position);
	float scale = 100000.f;
	vec4 right_point = thickness * scale * (calc_gl_position(vertex_position + direction / scale) - center_point);
	vec4 top_point = thickness * scale * (calc_gl_position(vertex_position + normal_dir / scale) - center_point);
	vec4 temp_result = center_point + (corner_direction.x * right_point + corner_direction.y * top_point);
	float adj_thickness = thickness * 100000.f;

	float pixels_thickness = thickness * map_size.x * zoom;
	float min_thickness = 0.5f;
	float max_thickness = 2000.f;
	opacity = clamp((1.f - min_thickness / pixels_thickness) * (pixels_thickness / max_thickness - 1.f), 0.f, 1.f);

	temp_result.z = 0.01f / (1.f * adj_thickness * zoom);
	gl_Position = temp_result;
	uv = vertexUV;
	bufferIndex = vertexIndex;
}
