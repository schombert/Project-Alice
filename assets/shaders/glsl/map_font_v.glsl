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

vec4 calc_gl_position(vec2 position) {
	return point_to_ogl_space(int(subroutines_index), position, map_size, rotation, offset, aspect_ratio, zoom);
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



vec2 rotate_left_rect(vec2 v) {
	vec2 scaled = v * map_size;
	vec2 rotated = vec2(-scaled.y, scaled.x);
	return rotated / map_size;
}



void main() {
	vec2 normal_dir = rotate_left(int(subroutines_index), vertex_position, direction, map_size);
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
