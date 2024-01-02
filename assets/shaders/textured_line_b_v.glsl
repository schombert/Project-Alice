// Goes from 0 to 1
layout (location = 0) in vec2 vertex_position;
layout (location = 1) in vec2 prev_point;
layout (location = 2) in vec2 next_point;
layout (location = 3) in float texture_coord;
layout (location = 4) in float distance;

out float tex_coord;
out float o_dist;
out vec2 map_coord;

layout (location = 0) uniform vec2 offset;
layout (location = 1) uniform float aspect_ratio;
layout (location = 2) uniform float zoom;
layout (location = 3) uniform vec2 map_size;
layout (location = 4) uniform float width;
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
		(2. * new_world_pos.x - 1.f)  * zoom,
		(2. * new_world_pos.z - 1.f) * zoom,
		(2. * new_world_pos.y - 1.f) - 1.0f,
		1.0);
}

layout(index = 1) subroutine(calc_gl_position_class)
vec4 flat_coords(vec2 world_pos) {
	world_pos += vec2(-offset.x, offset.y);
	world_pos.x = mod(world_pos.x, 1.0f);
	return vec4(
		(2. * world_pos.x - 1.f) * zoom * map_size.x / map_size.y,
		(2. * world_pos.y - 1.f) * zoom,
		abs(world_pos.x - 0.5) * 2.1f,
		1.0);
}

void main() {
	vec4 central_pos = calc_gl_position(vertex_position);
	vec2 bpt = central_pos.xy;
	vec2 apt = calc_gl_position(prev_point).xy;
	vec2 cpt = calc_gl_position(next_point).xy;
	
	vec2 adir = normalize(bpt - apt);
	vec2 bdir = normalize(cpt - bpt);
	
	vec2 anorm = vec2(-adir.y, adir.x);
	vec2 bnorm = vec2(-bdir.y, bdir.x);
	vec2 corner_normal = (anorm + bnorm) / (1.0f + max(-0.5f, dot(anorm, bnorm))) * zoom * width;
	
	gl_Position = central_pos + vec4(corner_normal.x, corner_normal.y, 0.0f, 0.0f);
	gl_Position.x /= aspect_ratio;
	tex_coord = texture_coord;
	o_dist = distance / (2.0f * width);
	map_coord = vertex_position;
}
