// Goes from 0 to 1
layout (location = 0) in vec2 vertex_position;
layout (location = 1) in vec2 prev_point;
layout (location = 2) in vec2 next_point;
layout (location = 3) in vec2 province_index;
layout (location = 4) in float texture_coord;
layout (location = 5) in float distance;

out float tex_coord;
out float o_dist;
out vec2 map_coord;
out vec2 space_coord;
out float distance_to_national_border;
flat out vec2 frag_province_index;

uniform vec2 offset;
uniform float aspect_ratio;
uniform float zoom;
uniform vec2 map_size;
uniform float width;
uniform mat3 rotation;
uniform uint subroutines_index;
uniform float is_national_border;

vec4 calc_gl_position(vec2 position) {
	return point_to_ogl_space(int(subroutines_index), position, map_size, rotation, offset, aspect_ratio, zoom);
}

void main() {
	vec4 central_pos = calc_gl_position(vertex_position);
	vec2 corner_shift = vec2(0.f, 0.f);
	float true_width = width;

	vec2 bpt = central_pos.xy;
	vec2 prev = prev_point;
	vec2 next = next_point;
	if (prev == vertex_position) {
		prev = vertex_position + vertex_position - next_point;
	}
	if (next == vertex_position) {
		next = vertex_position + vertex_position - prev_point;
	}
	vec2 apt = calc_gl_position(prev).xy;
	vec2 cpt = calc_gl_position(next).xy;

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

	corner_shift = texture_coord * corner_normal * zoom * true_width; /// (1.0f + max(-0.5f, dot(anorm, bnorm)));

	// transform result back to screen + depth coordinates
	corner_shift.x /= aspect_ratio;

	gl_Position = central_pos + vec4(corner_shift.x, corner_shift.y, 0.001f * abs(texture_coord), 0.0f);

	if(int(subroutines_index) == 1 || int(subroutines_index) == 3) {
		gl_Position.z = 0.001f * abs(texture_coord);
		if(central_pos.z < -1.f) {
			gl_Position.z = -1.05f;
		}
		//gl_Position.z += abs(gl_Position.x - 0.5) * 2.1f;
	}

	// pass data to frag shader
	tex_coord = abs(texture_coord);
	o_dist = distance / (2.0f * width);
	map_coord = vertex_position + corner_shift;
	space_coord = gl_Position.xy;
	frag_province_index = province_index;
	if (is_national_border > 0.5f) {
		distance_to_national_border = tex_coord;
		gl_Position.z -= 0.001f;
	} else {
		distance_to_national_border = 1.f;
	}
}
