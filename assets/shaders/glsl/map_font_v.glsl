// Goes from 0 to 1
layout (location = 0) in vec2 vertex_position;
layout (location = 1) in vec2 corner_direction;
layout (location = 2) in vec2 direction;
layout (location = 3) in vec2 vertexUV;
layout (location = 4) in float thickness;
layout (location = 5) in int vertexIndex;

out vec2 uv;
flat out int bufferIndex;

uniform vec2 offset;
uniform float aspect_ratio;
uniform float zoom;
uniform vec2 map_size;
uniform mat3 rotation;
uniform float time;
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
		(2. * new_world_pos.x - 1.f) / aspect_ratio  * zoom,
		(2. * new_world_pos.z - 1.f) * zoom,
		(2. * new_world_pos.y - 1.f), 1.0);
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


vec3 from_square(vec2 sq_point) {
	float psi = sq_point.x * 2.f * PI;
	float phi = (sq_point.y - 0.5f) * PI;
	return vec3(sin(phi), cos(phi) * cos(psi), cos(phi) * sin(psi));
};
vec2 to_square(vec3 sph_point) {
	float psi = atan(sph_point.z, sph_point.y);
	float phi = asin(sph_point.x);
	if(psi < 0.f) {
		psi += 2.f * PI;
	}
	return vec2(psi / PI / 2.f,	phi / PI + 0.5f);
}
vec3 tangent_from_square(vec2 square_point, vec2 square_tangent) {
	float psi = square_point.x * 2.f * PI;
	float phi = (square_point.y - 0.5f) * PI;

	mat2x3 differential;
	differential[0] = vec3(0.f, -cos(phi) * sin(psi), cos(phi) * cos(psi));
	differential[1] = vec3(cos(phi), -sin(phi) * cos(psi), -sin(phi) * sin(psi));

	return differential * (vec2(2.f * PI, PI) * square_tangent);
}
vec2 tangent_to_square(vec3 sphere_point, vec3 sph_tangent) {
	float psi = atan(sphere_point.z, sphere_point.y);
	float phi = asin(sphere_point.x);

	vec2 base = to_square(sphere_point);

	vec3 up = vec3(cos(phi), -sin(phi) * cos(psi), -sin(phi) * sin(psi));
	up /= length(up);
	vec3 right = vec3(0.f, -cos(phi) * sin(psi), cos(phi) * cos(psi));
	right /= length(right);

	float coordinate_up = dot(sph_tangent, up);
	float coordinate_right =  dot(sph_tangent, right);

	return vec2(coordinate_right / PI / 2.f / cos(phi), coordinate_up / PI);
}

vec2 rotate_left_sphere(vec2 point, vec2 tangent) {
	vec3 sphere_tangent = tangent_from_square(point, tangent);
	vec3 sphere_point = from_square(point);
	vec3 away = sphere_point;
	away /= length(away);
	vec3 rotated = cross(away, sphere_tangent);
	return tangent_to_square(sphere_point, rotated);
}

vec2 rotate_left_rect(vec2 v) {
	vec2 scaled = v * map_size;
	vec2 rotated = vec2(-scaled.y, scaled.x);
	return rotated / map_size;
}

vec2 rotate_left(vec2 point, vec2 tangent) {
	switch(int(subroutines_index)) {

	case 0: return rotate_left_sphere(point, tangent);
	case 1: return rotate_left_sphere(point, tangent);
	case 2: return rotate_left_rect(tangent);
	default: break;

	}
	return vec2(0.f);
}

void main() {
	vec2 normal_dir = rotate_left(vertex_position, direction);

	vec4 center_point = calc_gl_position(vertex_position);
	float scale = 100000.f;
	vec4 right_point = thickness * scale * (calc_gl_position(vertex_position + direction / scale) - center_point);

	vec4 top_point = thickness * scale * (calc_gl_position(vertex_position + normal_dir / scale) - center_point);

	vec4 temp_result = center_point + (corner_direction.x * right_point + corner_direction.y * top_point);
    
    float opacity = 1.f;    
    opacity = exp(-(zoom * 50.f - 1.f/thickness) * (zoom * 50.f - 1.f/thickness) * 0.000001f);
        
	temp_result.z = 0.01f / (1.f * thickness * zoom) / 100000.f;
   
	gl_Position = temp_result;
	uv = vertexUV;
	bufferIndex = vertexIndex;
}
