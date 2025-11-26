#include "projections.hpp"

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
		{
			x.data.x * size_x,
			x.data.y * size_y
		}
	};
}
square::tangent to_square(tangent x, float size_x, float size_y) {
	return {
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
	return to_square((tangent{ rotate_left_internal(from_square(x, size_x, size_y).data) }), size_x, size_y);
}
square::tangent rotate_right(square::tangent x, float size_x, float size_y) {
	return to_square((tangent{ rotate_right_internal(from_square(x, size_x, size_y).data) }), size_x, size_y);
}
}
