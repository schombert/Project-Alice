// Goes from 0 to 1
layout (location = 0) in vec2 vertex_position;
layout (location = 1) in vec2 prev_point;
layout (location = 2) in vec2 next_point;
layout (location = 3) in float texture_coord;
layout (location = 4) in float distance;

out float tex_coord;
out float o_dist;
out vec2 map_coord;

uniform vec2 offset;
uniform float aspect_ratio;
uniform float zoom;
uniform vec2 map_size;
uniform float width;
uniform mat3 rotation;
uniform uint subroutines_index;

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
		(2. * new_world_pos.x - 1.f) * zoom / aspect_ratio,
		(2. * new_world_pos.z - 1.f) * zoom,
		(2. * new_world_pos.y - 1.f) - 1.0f,
		1.0);
}

vec4 flat_coords(vec2 world_pos) {
	world_pos += vec2(-offset.x, offset.y);
	world_pos.x = mod(world_pos.x, 1.0f);
	return vec4(
		(2. * world_pos.x - 1.f) * zoom / aspect_ratio * map_size.x / map_size.y,
		(2. * world_pos.y - 1.f) * zoom,
		abs(world_pos.x - 0.5) * 2.1f,
		1.0);
}

vec4 perspective_coords(vec2 world_pos) {
	vec3 new_world_pos;
	float angle_x = 2 * world_pos.x * PI;
	new_world_pos.x = cos(angle_x);
	new_world_pos.y = sin(angle_x);
	float angle_y = world_pos.y * PI;
	new_world_pos.x *= sin(angle_y);
	new_world_pos.y *= sin(angle_y);
	new_world_pos.z = cos(angle_y);


	new_world_pos = rotation * new_world_pos;
	new_world_pos /= PI; // Will make the zoom be the same for the globe and flat map
	new_world_pos.xz *= -1;
	new_world_pos.zy = new_world_pos.yz;


	new_world_pos.x /= aspect_ratio;
	new_world_pos.z -= 1.2;
	float near = 0.1;
	float tangent_length_square = 1.2f * 1.2f - 1 / PI / PI;
	float far = tangent_length_square / 1.2f;

	float right = near * tan(PI / 6) / zoom;
	float top = near * tan(PI / 6) / zoom;
	new_world_pos.x *= near / right;
	new_world_pos.y *= near / top;
	float w = -new_world_pos.z;
	new_world_pos.z = -(far + near) / (far - near) * new_world_pos.z - 2 * far * near / (far - near);
	return vec4(new_world_pos, w);
}

vec4 calc_gl_position(vec2 world_pos) {
	switch(int(subroutines_index)) {
case 0: return globe_coords(world_pos);
case 1: return perspective_coords(world_pos);
case 2: return flat_coords(world_pos);
default: break;
	}
	return vec4(0.f);
}

void main() {
	vec4 central_pos = calc_gl_position(vertex_position);
	vec2 bpt = central_pos.xy;
	vec2 apt = calc_gl_position(prev_point).xy;
	vec2 cpt = calc_gl_position(next_point).xy;

	// we want to thicken the line in "perceived" coordinates, so
	// transform to perceived coordinates + depth
	bpt.x *= aspect_ratio;
	apt.x *= aspect_ratio;
	cpt.x *= aspect_ratio;

	// calculate normals in perceived coordinates + depth
	vec2 adir = normalize(bpt - apt);
	vec2 bdir = normalize(cpt - bpt);

	vec2 anorm = vec2(-adir.y, adir.x);
	vec2 bnorm = vec2(-bdir.y, bdir.x);
	vec2 corner_normal = normalize(anorm + bnorm);

	vec2 corner_shift = corner_normal * zoom * width / (1.0f + max(-0.5f, dot(anorm, bnorm)));

	// transform result back to screen + depth coordinates
	corner_shift.x /= aspect_ratio;

	gl_Position = central_pos + vec4(corner_shift.x, corner_shift.y, 0.0f, 0.0f);

	// pass data to frag shader
	tex_coord = texture_coord;
	o_dist = distance / (2.0f * width);
	map_coord = vertex_position;
}
