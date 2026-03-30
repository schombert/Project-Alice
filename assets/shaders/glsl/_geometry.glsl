/*
This file is included into all shaders automatically
Functions in this file are shader versions of functions from projections.cpp
*/

#version 460 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_shader_subroutine : enable
#extension GL_ARB_vertex_array_object : enable
#define M_PI 3.1415926535897932384626433832795
#define PI 3.1415926535897932384626433832795

struct square_tangent {vec2 base; vec2 tangent;};
struct rectangle_tangent {vec2 base; vec2 tangent;};
struct sphere_tangent { vec3 base; vec3 tangent; };
struct stereographic_tangent { vec2 base; vec2 tangent; };

vec2 rotate_left(vec2 input_vec) { return vec2( -input_vec.y, input_vec.x ); }
vec2 rotate_right(vec2 input_vec) { return vec2( input_vec.y, -input_vec.x ); }

square_tangent rotate_left(square_tangent input_vec) { return square_tangent( input_vec.base, vec2( -input_vec.tangent.y, input_vec.tangent.x ) ); }
square_tangent rotate_right(square_tangent input_vec) { return square_tangent( input_vec.base, vec2( input_vec.tangent.y, -input_vec.tangent.x ) ); }

vec2 point_to_equirectangular(vec2 point, vec2 map_size) {
	return point * map_size / map_size.y;
}
rectangle_tangent tangent_to_equirectangular(square_tangent val, vec2 map_size) {
	return rectangle_tangent(val.base * map_size / map_size.y, val.tangent * map_size / map_size.y);
}

vec2 point_from_equirectangular(vec2 point, vec2 map_size) {
	return point / map_size * map_size.y;
}
square_tangent tangent_from_equirectangular(rectangle_tangent val, vec2 map_size) {
	return square_tangent(val.base / map_size * map_size.y, val.tangent / map_size * map_size.y);
}

rectangle_tangent rotate_left(rectangle_tangent val) {
	return rectangle_tangent(val.base, rotate_left(val.tangent));
}

rectangle_tangent rotate_right(rectangle_tangent val) {
	return rectangle_tangent(val.base, rotate_right(val.tangent));
}

vec3 point_to_sphere(vec2 point) {
	float psi = point.x * 2.f * PI;
	float phi = (point.y - 0.5f) *PI;
	return vec3(sin(phi), cos(phi) * cos(psi), cos(phi) * sin(psi));
};

vec2 sphere_to_point(vec3 point) {
	float psi = atan(point.z, point.y);
	float phi = asin(point.x);
	return vec2(psi / PI / 2.f, phi / PI + 0.5f);
};

sphere_tangent tangent_to_sphere_tangent(square_tangent val) {
	float psi = val.base.x * 2.f * PI;
	float phi = (val.base.y - 0.5f) *PI;
	vec3 base = point_to_sphere(val.base);
	mat2x3 differential = mat2x3(
		vec3( 0.f, -cos(phi) * sin(psi), cos(phi)* cos(psi) ),
		vec3( cos(phi), -sin(phi) * cos(psi), -sin(phi) * sin(psi) )
	); 
	return sphere_tangent(base, differential* (vec2(2.f * PI, PI)* val.tangent));
};

square_tangent sphere_tangent_to_tangent(sphere_tangent  val) {
	float psi = atan(val.base.z, val.base.y);
	float phi = asin(val.base.x);
	vec2 base = sphere_to_point(val.base);
	vec3 up = vec3( cos(phi), -sin(phi) * cos(psi), -sin(phi) * sin(psi) );
	vec3 right = vec3( 0.f, -cos(phi) * sin(psi), cos(phi) * cos(psi) );
	up = up / length(up);
	right = right / length(right);
	float coordinate_up = dot(val.tangent, up);
	float coordinate_right = dot(val.tangent, right);
	return square_tangent(
		base,
		vec2(
			coordinate_right / PI / 2.f / cos(phi),
			coordinate_up / PI
		)
	);
};

square_tangent rotate_left_sphere(square_tangent tangent) {
	sphere_tangent sphere = tangent_to_sphere_tangent(tangent);
	sphere_tangent rotated = sphere_tangent(sphere.base, cross(sphere.base, sphere.tangent));
	return sphere_tangent_to_tangent(rotated);
}

vec2 rotate_left(int mode, vec2 point, vec2 tangent_vector, vec2 map_size) {
	square_tangent tangent = square_tangent(point, tangent_vector);

	switch(mode) {
		case 0: return rotate_left_sphere(tangent).tangent;
		case 1: return tangent_from_equirectangular(rotate_left(tangent_to_equirectangular(tangent, map_size)), map_size).tangent;
		case 2: return rotate_left_sphere(tangent).tangent;
		case 3: return rotate_left_sphere(tangent).tangent;
		default: break;
	}

	return vec2(0.f);
}


/*

This function converts a square point into the corresponding point on currently desired manifold and converts it into camera space

Camera is shifting if camera transformation applies parallel shift. This kind of camera is associated with flat plane modes.
Camera is rotating if cameta transformation applies rotation. This kind of camera is associated with globe modes

Camera is flat if we do a parallel linear projection.
Camera is perspective if we do perspective transformation.

Currently implemented projections are:

0) Sphere 2. Rotating flat camera.
1) Rectangle. Shifting flat camera.
2) Sphere 1. Rotating perspective camera.
3) Sphere 3. Rotating stereographic camera.

*/

vec4 rectangle_to_ogl(vec2 plane_position, vec2 camera_offset, vec2 map_size, float aspect_ratio, float zoom) {
	float map_size_ratio = map_size.x / map_size.y;
	float cut_away = 1.05f;
	vec2 pos = plane_position - camera_offset;

	// Wrap around and center at 0
	pos.x = mod(pos.x + map_size_ratio * 0.5f, map_size_ratio) - map_size_ratio * 0.5f;
	return vec4(
		2. * pos.x * zoom / aspect_ratio,
		2. * pos.y * zoom,
		abs(2.f * pos.x) / map_size_ratio * cut_away,
		1.0
	);
}

vec4 sphere_to_ogl_perspective(vec3 p, mat3 rotation, float aspect_ratio, float zoom) {
	p.xy = p.yx;
	vec3 rotated_p = rotation * p;
	rotated_p /= PI; // Will make the zoom be the same for the globe and flat map

	rotated_p.z -= 1.2f;

	float near = 0.1;
	float tangent_length_square = 1.2f * 1.2f - 1.f / PI / PI;
	float far = tangent_length_square / 1.2f;

	float right = near * tan(PI / 6) / zoom * aspect_ratio;
	float top = near * tan(PI / 6) / zoom;

	rotated_p.x *= near / right;
	rotated_p.y *= near / top;

	float w = -rotated_p.z;

	rotated_p.z = -(far + near) / (far - near) * rotated_p.z - 2 * far * near / (far - near);

	return vec4(rotated_p, w);
}

vec4 sphere_to_ogl_parallel(vec3 p, mat3 rotation, float aspect_ratio, float zoom) {
	p.xy = p.yx;
	vec3 new_world_pos = p;
	new_world_pos = rotation * new_world_pos;
	new_world_pos /= PI; // Will make the zoom be the same for the globe and flat map

	return vec4(
		2. * new_world_pos.x / aspect_ratio * zoom,
		2. * new_world_pos.y * zoom,
		new_world_pos.z - 1.f,
		1.0
	);
}

vec4 sphere_to_ogl_stereographic(vec3 p, mat3 rotation, float aspect_ratio, float zoom) {
	p.xy = p.yx;
	vec3 new_world_pos = p;
	new_world_pos = rotation * new_world_pos;
	new_world_pos.z += 1.f;
	return vec4(
		0.5f * new_world_pos.x / new_world_pos.z / aspect_ratio * zoom,
		0.5f * new_world_pos.y / new_world_pos.z * zoom,
		1.01f - new_world_pos.z,
		1.0
	);
}

vec4 point_to_ogl_space(int mode, vec2 p, vec2 map_size, mat3 rotation, vec2 offset, float aspect_ratio, float zoom) {
	vec3 sphere_point = point_to_sphere(p);
	
	switch(mode) {
		case 0:
		return sphere_to_ogl_parallel(sphere_point, rotation, aspect_ratio, zoom);

		case 1:
		return rectangle_to_ogl(point_to_equirectangular(p, map_size), point_to_equirectangular(offset, map_size), map_size, aspect_ratio, zoom);

		case 2:
		return sphere_to_ogl_perspective(sphere_point, rotation, aspect_ratio, zoom);

		case 3:
		return sphere_to_ogl_stereographic(sphere_point, rotation, aspect_ratio, zoom);

		default:
		break;
	}
	return vec4(0.f);
}
