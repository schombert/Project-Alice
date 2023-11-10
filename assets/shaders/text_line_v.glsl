// Goes from 0 to 1
layout (location = 0) in vec2 vertex_position;
layout (location = 1) in vec2 normal_direction;
layout (location = 2) in vec2 direction;
layout (location = 3) in vec2 texture_coord;
layout (location = 4) in float codepoint_tn;
layout (location = 5) in float thickness;

out vec2 tex_coord;
out float type;

// Camera position
layout (location = 0) uniform vec2 offset;
layout (location = 1) uniform float aspect_ratio;
// Zoom: big numbers = close
layout (location = 2) uniform float zoom;
// The size of the map in pixels
layout (location = 3) uniform vec2 map_size;
layout (location = 5) uniform mat3 rotation;

subroutine vec4 calc_gl_position_class(vec2 world_pos);
subroutine uniform calc_gl_position_class calc_gl_position;

layout(index = 0) subroutine(calc_gl_position_class)
vec4 globe_coords(vec2 world_pos) {

	vec3 new_world_pos;
	float section_x = 200;
	float angle_x1 = 2 * PI * floor(world_pos.x * section_x) / section_x;
	float angle_x2 = 2 * PI * floor(world_pos.x * section_x + 1) / section_x;
	float angle_x = mix(angle_x1, angle_x2, mod(world_pos.x * section_x, 1));
	new_world_pos.x = mix(cos(angle_x1), cos(angle_x2), mod(world_pos.x * section_x, 1));
	new_world_pos.y = mix(sin(angle_x1), sin(angle_x2), mod(world_pos.x * section_x, 1));
	float section_y = 200;
	float angle_y1 = PI * floor(world_pos.y * section_y) / section_y;
	float angle_y2 = PI * floor(world_pos.y * section_y + 1) / section_y;
	new_world_pos.x *= mix(sin(angle_y1), sin(angle_y2), mod(world_pos.y * section_y, 1));
	new_world_pos.y *= mix(sin(angle_y1), sin(angle_y2), mod(world_pos.y * section_y, 1));
	new_world_pos.z = mix(cos(angle_y1), cos(angle_y2), mod(world_pos.y * section_y, 1));
	new_world_pos = rotation * new_world_pos;
	new_world_pos /= PI; 		// Will make the zoom be the same for the globe and flat map
	new_world_pos.y *= 0.02; 	// Sqeeze the z coords. Needs to be between -1 and 1
	new_world_pos.xz *= -1; 	// Invert the globe
	new_world_pos.xyz += 0.5; 	// Move the globe to the center

	return vec4(
		(2. * new_world_pos.x - 1.f) * zoom,
		(2. * new_world_pos.z - 1.f) * zoom,
		(2. * new_world_pos.y - 1.f), 1.0);
}

layout(index = 1) subroutine(calc_gl_position_class)
vec4 flat_coords(vec2 world_pos) {
	world_pos += vec2(-offset.x, offset.y);
	world_pos.x = mod(world_pos.x, 1.0f);
	return vec4(
		(2. * world_pos.x - 1.f) * zoom * map_size.x / map_size.y,
		(2. * world_pos.y - 1.f) * zoom,
		0.0, 1.0);
}

// The borders are drawn by seperate quads.
// Each triangle in the quad is made up by two vertices on the same position and
// another one in the "direction" vector. Then all the vertices are offset in the "normal_direction".
void main() {
	//vec2 rot_direction = vec2(-direction.y, direction.x);
	//vec2 normal_vector = normalize(normal_direction) * thickness;
	//vec2 extend_vector = -normalize(direction) * thickness;
	
	//vec2 world_pos = vertex_position;


	float x_adj = 1.0f / aspect_ratio;
	
	vec2 unadj_direction = vec2(direction.x / 2.0f, direction.y);
	vec2 unadj_normal = vec2(-direction.y / 2.0f, direction.x);
	
	vec4 center_point = calc_gl_position(vertex_position);
	vec4 right_point = thickness * 10000 * (calc_gl_position(vertex_position + unadj_direction * 0.0001) - center_point);
	
	vec4 top_point = thickness * 10000 * (calc_gl_position(vertex_position + unadj_normal * 0.0001) - center_point);
	
	//vec2 offset = normal_vector + extend_vector;
	//world_pos += offset * scale;
	
	vec4 temp_result = center_point + (normal_direction.x * right_point + normal_direction.y * top_point);
	temp_result.x *= x_adj;
	temp_result.z = 0.01f / (thickness * zoom);

	gl_Position = temp_result;
	tex_coord = texture_coord;
	type = codepoint_tn;
}
