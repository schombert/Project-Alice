#pragma once

#include "dcon_generated_ids.hpp"

namespace ogl {

struct color3f {
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
};

}

namespace sys {

struct text_mouse_test_result {
	uint32_t position;
	uint32_t quadrent;
};

}


namespace ui {
class element_base;

struct xy_pair {
	int16_t x = 0;
	int16_t y = 0;
};
static_assert(sizeof(xy_pair) == 4);
struct urect {
	xy_pair top_left;
	xy_pair size;
};

struct mouse_probe {
	element_base* under_mouse;
	xy_pair relative_location;
};

struct error_window {
	std::string header_text;
	std::string description_text;

};

}
