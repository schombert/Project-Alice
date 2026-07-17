// Goes from 0 to 1
layout (location = 0) in vec2 vertex_position;
layout (location = 1) in vec2 vertexUV;
layout (location = 2) in float thickness;
layout (location = 3) in int vertexIndex;

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
	vec4 temp_result = calc_gl_position(vertex_position);

	float pixels_thickness = thickness * zoom;
	float min_thickness = 0.005f;
	float max_thickness = 1.f;
	opacity = clamp((min_thickness / pixels_thickness - 1.f) * (pixels_thickness / max_thickness - 1.f), 0.f, 1.f);

	temp_result.z = 0.01f / (1.f * thickness * 100000.f * zoom);
	gl_Position = temp_result;
	uv = vertexUV;
	bufferIndex = vertexIndex;
}
