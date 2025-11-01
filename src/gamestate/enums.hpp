#pragma once

namespace sys {
enum class gui_modes : uint8_t {
	faithful = 0, nouveau = 1, dummycabooseval = 2
};
enum class projection_mode : uint8_t {
	globe_ortho = 0, flat = 1, globe_perpect = 2, num_of_modes = 3
};
struct text_mouse_test_result {
	uint32_t position;
	uint32_t quadrent;
};
}
