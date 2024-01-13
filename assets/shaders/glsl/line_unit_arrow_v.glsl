// Goes from 0 to 1
layout (location = 0) in vec2 vertex_position;
layout (location = 1) in vec2 normal_direction;
layout (location = 2) in vec2 direction;
layout (location = 3) in vec2 texture_coord;
layout (location = 4) in float type;

out vec2 tex_coord;
// Camera position
layout (location = 0) uniform vec2 offset;
layout (location = 1) uniform float aspect_ratio;
// Zoom: big numbers = close
layout (location = 2) uniform float zoom;
// The size of the map in pixels
layout (location = 3) uniform vec2 map_size;
// The scaling factor for the width
layout (location = 4) uniform float border_width;
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
		(2. * new_world_pos.x - 1.f) / aspect_ratio * zoom,
		(2. * new_world_pos.z - 1.f) * zoom,
		(2. * new_world_pos.y - 1.f), 1.0);
}


layout(index = 1) subroutine(calc_gl_position_class)
vec4 flat_coords(vec2 world_pos) {
	world_pos += vec2(-offset.x, offset.y);
	world_pos.x = mod(world_pos.x, 1.0f);
	return vec4(
		(2. * world_pos.x - 1.f) * zoom / aspect_ratio * map_size.x / map_size.y,
		(2. * world_pos.y - 1.f) * zoom,
		abs(world_pos.x - 0.5) * 2.1f,
		1.0);
}

layout(index = 2) subroutine(calc_gl_position_class)
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


// The borders are drawn by seperate quads.
// Each triangle in the quad is made up by two vertices on the same position and
// another one in the "direction" vector. Then all the vertices are offset in the "normal_direction".
void main() {
	float thickness = border_width * 0.25f;
	vec2 rot_direction = vec2(-direction.y, direction.x);
	vec2 normal_vector = normalize(normal_direction) * thickness;
	// Extend the border slightly to make it fit together with any other border in any octagon direction.
	vec2 extend_vector = -normalize(direction) * thickness;
	extend_vector *= round(type) == 3.f ? 2.f : 0.f;
	vec2 world_pos = vertex_position;


	world_pos.x *= map_size.x / map_size.y;
	world_pos += normal_vector;
	world_pos += extend_vector;
	world_pos.x /= map_size.x / map_size.y;

	gl_Position = calc_gl_position(world_pos);
	tex_coord = texture_coord / 2.f + vec2(0.5 * mod(round(type), 2.f), 0.5 * floor(mod(type, 3.0f) / 2.f));
}
