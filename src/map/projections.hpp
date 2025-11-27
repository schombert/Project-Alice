#pragma once
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace screen {
struct point {
	glm::vec2 data;
};
struct tangent {
	glm::vec2 data;
};
}

namespace ui_space {
struct point {
	glm::vec2 data;
};
struct tangent {
	glm::vec2 data;
};
}

namespace square {
struct point {
	glm::vec2 data; // from 0 to 1
};
struct tangent {
	glm::vec2 data;
};
}

namespace world {
struct point {
	glm::vec3 data;
};
struct tangent {
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
struct point;
struct tangent;
point from_square(square::point x, float size_x, float size_y);
square::point to_square(point x, float size_x, float size_y);
tangent from_square(square::tangent x, float size_x, float size_y);
square::tangent to_square(tangent x, float size_x, float size_y);
float dot(square::tangent x, square::tangent y, float size_x, float size_y);
square::tangent rotate_left(square::tangent x, float size_x, float size_y);
square::tangent rotate_right(square::tangent x, float size_x, float size_y);
}
