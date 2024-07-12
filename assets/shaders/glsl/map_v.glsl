// Goes from 0 to 1
layout (location = 0) in vec2 vertex_position;
out vec2 tex_coord;

// Camera position
uniform vec2 offset;
uniform float aspect_ratio;
// Zoom: big numbers = close
uniform float zoom;
// The size of the map in pixels
uniform vec2 map_size;
uniform mat3 rotation;
uniform uint subroutines_index;

vec4 globe_coords() {
	vec3 new_world_pos;
	float angle_x = 2 * vertex_position.x * PI;
	new_world_pos.x = cos(angle_x);
	new_world_pos.y = sin(angle_x);

	float angle_y = vertex_position.y * PI;
	new_world_pos.x *= sin(angle_y);
	new_world_pos.y *= sin(angle_y);
	new_world_pos.z = cos(angle_y);
	new_world_pos = rotation * new_world_pos;
	new_world_pos /= PI; 		// Will make the zoom be the same for the globe and flat map
	new_world_pos.y *= 0.02; 	// Sqeeze the z coords. Needs to be between -1 and 1
	new_world_pos.xz *= -1; 	// Invert the globe
	new_world_pos.xyz += 0.5; 	// Move the globe to the center

	return vec4(
		(2. * new_world_pos.x - 1.f) / aspect_ratio  * zoom,
		(2. * new_world_pos.z - 1.f) * zoom,
		(2. * new_world_pos.y - 1.f) - 1.0f,
		1.0);
}

vec4 flat_coords() {
	vec2 world_pos = vertex_position + vec2(-offset.x, offset.y);
	world_pos.x = mod(world_pos.x, 1.0f);

	return vec4(
		(2. * world_pos.x - 1.f) * zoom / aspect_ratio * map_size.x / map_size.y,
		(2. * world_pos.y - 1.f) * zoom,
		abs(world_pos.x - 0.5) * 2.1f,
		1.0);
}

vec4 perspective_coords() {
	vec3 new_world_pos;
	float angle_x = 2 * vertex_position.x * PI;
	new_world_pos.x = cos(angle_x);
	new_world_pos.y = sin(angle_x);

	float angle_y = vertex_position.y * PI;
	new_world_pos.x *= sin(angle_y);
	new_world_pos.y *= sin(angle_y);
	new_world_pos.z = cos(angle_y);

	new_world_pos = rotation * new_world_pos;
	new_world_pos /= PI; // Will make the zoom be the same for the globe and flat map
	new_world_pos.xz *= -1;
	new_world_pos.zy = new_world_pos.yz;

	new_world_pos.x /= aspect_ratio;
	new_world_pos.z -= 1.2f;
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

vec4 calc_gl_position() {
	switch(int(subroutines_index)) {
case 0: return globe_coords();
case 1: return perspective_coords();
case 2: return flat_coords();
default: break;
	}
	return vec4(0.f);
}

void main() {
	gl_Position = calc_gl_position();
	tex_coord = vertex_position;
}
