// Goes from 0 to 1
layout (location = 0) in vec2 vertex_position;
layout (location = 1) in vec2 normal_direction;
layout (location = 2) in vec2 direction;
layout (location = 3) in vec3 texture_coord;
layout (location = 4) in float thickness;

out vec3 tex_coord;
out float opacity;
out float text_size;

// Camera position
uniform vec2 offset;
uniform float aspect_ratio;
// Zoom: big numbers = close
uniform float zoom;
// The size of the map in pixels
uniform vec2 map_size;
uniform mat3 rotation;
uniform float opaque;
uniform uint subroutines_index;

vec4 calc_gl_position(vec2 position) {
	return point_to_ogl_space(int(subroutines_index), position, map_size, rotation, offset, aspect_ratio, zoom);
}

// The borders are drawn by seperate quads.
// Each triangle in the quad is made up by two vertices on the same position and
// another one in the "direction" vector. Then all the vertices are offset in the "normal_direction".
void main() {
	//vec2 rot_direction = vec2(-direction.y, direction.x);
	//vec2 normal_vector = normalize(normal_direction) * thickness;
	//vec2 extend_vector = -normalize(direction) * thickness;

	//vec2 world_pos = vertex_position;

	vec2 unadj_direction = vec2(direction.x / 2.0f, direction.y);
	vec2 unadj_normal = vec2(-direction.y / 2.0f, direction.x);

	vec4 center_point = calc_gl_position(vertex_position);
	vec4 right_point = thickness * 10000 * (calc_gl_position(vertex_position + unadj_direction * 0.0001) - center_point);

	vec4 top_point = thickness * 10000 * (calc_gl_position(vertex_position + unadj_normal * 0.0001) - center_point);

	//vec2 offset = normal_vector + extend_vector;
	//world_pos += offset * scale;

	vec4 temp_result = center_point + (normal_direction.x * right_point + normal_direction.y * top_point);
    
    opacity = 1.f;    
    if (opaque < 0.5f)
        opacity = exp(-(zoom * 50.f - 1.f/thickness) * (zoom * 50.f - 1.f/thickness) * 0.000001f);
        
	temp_result.z = 0.01f / (opacity * thickness * zoom) / 100000.f;
    
    text_size = thickness * zoom;

	gl_Position = temp_result;
	tex_coord = texture_coord;
}
