// Goes from 0 to 1
layout (location = 0) in vec2 vertex_position;
layout (location = 1) in vec2 normal_direction;
layout (location = 2) in float texture_coord;
layout (location = 3) in float distance;
layout (location = 4) in float width;

out float tex_coord;
out float o_dist;
out float width_factor;
out vec2 map_coord;
out vec3 space_coords;

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
	float adj_width = (width * (1.f / zoom) + 300.f * (1.f - 1.f / zoom));
	vec2 normal_vector = normalize(normal_direction) * adj_width * 0.000001f;
	vec2 world_pos = vertex_position;

	world_pos.x *= map_size.x / map_size.y;
	world_pos += normal_vector;
	world_pos.x /= map_size.x / map_size.y;

	map_coord = world_pos;

	vec4 temp = calc_gl_position(world_pos);

	float angle_x = 2 * vertex_position.x * PI;
	float x = cos(angle_x);
	float y = sin(angle_x);
	float angle_y = vertex_position.y * PI;
	x *= sin(angle_y);
	y *= sin(angle_y);
	float z = cos(angle_y);
	space_coords = vec3(x, y, z);
    
	gl_Position = temp;
	tex_coord = texture_coord * adj_width;
	width_factor = adj_width;
	o_dist = -time + distance * 50.f;
}
