#include "projections.hpp"

#include <numbers>
#include <glm/glm.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/polar_coordinates.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/transform.hpp>

glm::vec2 rotate_left_internal(glm::vec2 x) {
	return { -x.y, x.x };
}
glm::vec2 rotate_right_internal(glm::vec2 x) {
	return { x.y, -x.x };
}

namespace identity {

square::point from_square(square::point x) {
	return x;
}
square::point to_square(square::point x) {
	return x;
}
square::tangent from_square(square::tangent x) {
	return x;
}
square::tangent to_square(square::tangent x) {
	return x;
}

float dot(square::tangent x, square::tangent y) {
	return glm::dot(x.data, y.data);
}

}

namespace equirectangular {
struct point {
	glm::vec2 data;
};
struct tangent {
	point base;
	glm::vec2 data;
};
point from_square(square::point x, float size_x, float size_y) {
	return {
		{
			x.data.x * size_x,
			x.data.y * size_y
		}
	};
}
square::point to_square(point x, float size_x, float size_y) {
	return {
		{
			x.data.x / size_x,
			x.data.y / size_y
		}
	};
}
tangent from_square(square::tangent x, float size_x, float size_y) {
	return {
		from_square(x.base, size_x, size_y),
		{
			x.data.x * size_x,
			x.data.y * size_y
		}
	};
}
square::tangent to_square(tangent x, float size_x, float size_y) {
	return {
		to_square(x.base, size_x, size_y),
		{
			x.data.x / size_x,
			x.data.y / size_y
		}
	};
}
float dot(square::tangent x, square::tangent y, float size_x, float size_y) {
	return glm::dot(from_square(x, size_x, size_y).data, from_square(y, size_x, size_y).data);
}
square::tangent rotate_left(square::tangent x, float size_x, float size_y) {
	auto rect_tangent = from_square(x, size_x, size_y);
	tangent rotated = tangent{ rect_tangent.base, rotate_left_internal(rect_tangent.data) };
	return to_square(rotated, size_x, size_y);
}
square::tangent rotate_right(square::tangent x, float size_x, float size_y) {
	auto rect_tangent = from_square(x, size_x, size_y);
	tangent rotated = tangent{ rect_tangent.base, rotate_right_internal(rect_tangent.data) };
	return to_square(rotated, size_x, size_y);
}

std::string shader_functions() {
	return
		"vec2 point_to_equirectangular(vec2 point, vec2 map_size) {\n"
		"	return point * map_size / map_size.x;\n"
		"}\n"
		"vec2 tangent_to_equirectangular(vec2 point, vec2 tangent, vec2 map_size) {\n"
		"	return tangent * map_size / map_size.x;\n"
		"}\n"
		"vec2 point_from_equirectangular(vec2 point, vec2 map_size) {\n"
		"	return point / map_size * map_size.x;\n"
		"}\n"
		"vec2 tangent_from_equirectangular(vec2 point, vec2 tangent, vec2 map_size) {\n"
		"	return tangent / map_size * map_size.x;\n"
		"}\n"
		"vec2 rotate_left_tangent_in_equirectangular(vec2 point, vec2 tangent, vec2 map_size) {\n"
		"	return tangent_from_equirectangular(point, rotate_left(tangent_to_equirectangular(point, tangent, map_size)), map_size);\n"
		"}\n"
		"vec2 rotate_right_tangent_in_equirectangular(vec2 point, vec2 tangent, vec2 map_size) {\n"
		"	return tangent_from_equirectangular(point, rotate_right(tangent_to_equirectangular(point, tangent, map_size)), map_size);\n"
		"}\n"
		"";
}
}

namespace sphere_R3 {
point from_square(square::point sq_point) {
	auto psi = sq_point.data.x * 2.f * std::numbers::pi_v<float>;
	auto phi = (sq_point.data.y - 0.5f) * std::numbers::pi_v<float>;
	return {
		{
			sin(phi),
			cos(phi) * cos(psi),
			cos(phi) * sin(psi)
		}
	};
};
square::point to_square(point sph_point) {
	auto psi = atan2(sph_point.data.z, sph_point.data.y);
	auto phi = asin(sph_point.data.x);
	if(psi < 0.f) {
		psi += 2.f * std::numbers::pi_v<float>;
	}
	return {
		{
			psi / std::numbers::pi_v<float> / 2.f,
			phi / std::numbers::pi_v<float> + 0.5f
		}
	};
}
tangent from_square(square::tangent sq_tangent) {
	auto phi = (sq_tangent.base.data.y - 0.5f) * std::numbers::pi_v<float>;
	auto psi = sq_tangent.base.data.x * 2.f * std::numbers::pi_v<float>;
	point base = from_square(sq_tangent.base);
	glm::mat2x3 differential {
		{ 0.f, -cos(phi) * sin(psi), cos(phi) * cos(psi) },
		{ cos(phi), -sin(phi) * cos(psi), -sin(phi) * sin(psi) }
	};

	return {
		base,
		differential * (glm::vec2(2.f * std::numbers::pi_v<float>, std::numbers::pi_v<float>) * sq_tangent.data)
	};
}
float dot(tangent x, tangent y) {
	assert(glm::distance(x.base.data, y.base.data) < 0.0001f);
	return glm::dot(x.data, y.data);
};
square::tangent to_square(tangent sph_tangent) {
	auto phi = asin(sph_tangent.base.data.x);
	auto psi = atan2(sph_tangent.base.data.z, sph_tangent.base.data.y);

	square::point base = to_square(sph_tangent.base);

	glm::vec3 up = { cos(phi), -sin(phi) * cos(psi), -sin(phi) * sin(psi) };
	up /= glm::length(up);
	glm::vec3 right = { 0.f, -cos(phi) * sin(psi), cos(phi) * cos(psi) };
	right /= glm::length(right);

	float coordinate_up = glm::dot(sph_tangent.data, up);
	float coordinate_right = glm::dot(sph_tangent.data, right);

	return {
		base,
		{
			coordinate_right / std::numbers::pi_v<float> / 2.f / cos(phi),
			coordinate_up / std::numbers::pi_v<float>
		}
	};
}
float dot(square::tangent x, square::tangent y) {
	return dot(from_square(x), from_square(y));
};
square::tangent rotate_left(square::tangent x) {
	auto sphere_tangent = from_square(x);
	auto away = sphere_tangent.base.data;
	away /= glm::length(away);
	tangent rotated = tangent{ sphere_tangent.base, { glm::cross(away, sphere_tangent.data) } };
	return to_square(rotated);
}
square::tangent rotate_right(square::tangent x) {
	auto sphere_tangent = from_square(x);
	auto away = sphere_tangent.base.data;
	away /= glm::length(away);
	tangent rotated = tangent{ sphere_tangent.base, { -glm::cross(away, sphere_tangent.data) } };
	return to_square(rotated);
}
std::string shader_functions() {
	return "";
};
}

namespace projection_shader {

namespace projections_list {
inline constexpr int square = 0;
inline constexpr int equirectangular = 1;
inline constexpr int ortho = 2;
inline constexpr int perspective = 3;
}

std::string vertex_prefix() {
	std::string common =
		"vec2 rotate_left(vec2 input_vec) { return vec2( -input_vec.y, input_vec.x ); }\n"
		"vec2 rotate_right(vec2 input_vec) { return vec2( input_vec.y, -input_vec.x ); }\n";
	return common + equirectangular::shader_functions() + sphere_R3::shader_functions();
}
}
