#pragma once
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace screen_space {
/*

# UI SPACE:
	[0, screen width] x [0, screen height]
	(0, 0) = top left

	In UI description and in many graphical APIs pixels are counted starting from the top left corner.
	We call this space UI space

# CLIP SPACE
	[-1, -1] x [-1, -1]
	(1, 1) --- top right.
	(-1, -1) --- bottom left

	In OpenGL this space appears after the projection step and before the rasterization step.
	It's useful when one doesn't care about actual sizes of the elements and only relative positions are important

# OGL SPACE
	[0, screen width] x [0, screen height]
	(0, 0) = bottom left

	After rasterisation, the origin shifts to the bottom left corner.

By default we would use UI space. It's not any better than any other choice, but it's good to have at least some convention.

*/

struct point_ui {
	glm::vec2 data;
};
struct point_ogl {
	glm::vec2 data;
};
struct point_clip_space {
	glm::vec2 data;
};
struct tangent {
	point_ui base;
	glm::vec2 data;
};

inline point_ui ui_from_clip_space(const point_clip_space& val, float screen_w, float screen_h) {
	return { { (val.data.x + 1.f) * 0.5f * screen_w, (1.f - val.data.y) * 0.5f * screen_h } };
}
inline point_clip_space clip_space_from_ui(const point_ui& val, float screen_w, float screen_h) {
	return { { (val.data.x / screen_w) * 2.f - 1.f, 1.f - (val.data.y / screen_h) * 2.f } };
}

}

namespace square {
struct point {
	glm::vec2 data; // from 0 to 1
};
struct tangent {
	point base;
	glm::vec2 data;
};
}

namespace map_space {

struct point { glm::vec2 data; };
using point_normalized = square::point;
struct point_normalized_inverted_y { glm::vec2 data; };

inline point_normalized_inverted_y inverted_from_normalized(const point_normalized& val) {
	return { { val.data.x, 1.f - val.data.y } };
}
inline point_normalized normalized_from_inverted(const point_normalized_inverted_y& val) {
	return { { val.data.x, 1.f - val.data.y } };
}
inline point_normalized_inverted_y inverted_from_point(const point& val, const float size_x, const float size_y) {
	return { { val.data.x / size_x, 1.f - val.data.y / size_y } };
}
inline square::point to_square(const point_normalized_inverted_y& val) {
	return { { val.data.x, 1.f - val.data.y } };
}
inline point to_point(const point_normalized_inverted_y& val, const float size_x, const float size_y) {
	return { { val.data.x * size_x, size_y - val.data.y * size_y} };
}
inline int32_t to_idx(const point_normalized_inverted_y& val, const float size_x, const float size_y) {
	return int32_t(size_y - val.data.y * size_y) * int32_t(size_x) + int32_t(val.data.x * size_x);
}

}

namespace ui_space {
struct point {
	glm::vec2 data;
};
struct tangent {
	point base;
	glm::vec2 data;
};
}



namespace world {
struct point {
	glm::vec3 data;
};
struct tangent {
	point base;
	glm::vec3 data;
};
}


namespace identity {
square::point from_square(square::point x);
square::point to_square(square::point x);
square::tangent from_square(square::tangent x);
square::tangent to_square(square::tangent x);
float dot(square::tangent x, square::tangent y);
}

namespace equirectangular {
struct point {
	glm::vec2 data;
};
struct tangent {
	point base;
	glm::vec2 data;
};
point from_square(square::point, float size_x, float size_y);
square::point to_square(point, float size_x, float size_y);
tangent from_square(square::tangent, float size_x, float size_y);
square::tangent to_square(tangent, float size_x, float size_y);
float dot(square::tangent, square::tangent, float size_x, float size_y);
square::tangent rotate_left(square::tangent, float size_x, float size_y);
square::tangent rotate_right(square::tangent, float size_x, float size_y);
}


namespace sphere_R3 {
struct point {
	glm::vec3 data;
};
struct tangent {
	point base;
	glm::vec3 data;
};
point from_square(square::point);
square::point to_square(point sph_point);
tangent from_square(square::tangent);
float dot(tangent x, tangent y);
square::tangent to_square(tangent sph_tangent);
float dot(square::tangent x, square::tangent y);
square::tangent rotate_left(square::tangent x);
square::tangent rotate_right(square::tangent x);
}
