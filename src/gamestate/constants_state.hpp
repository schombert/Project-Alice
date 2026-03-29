#pragma once

namespace sys {
enum class gui_modes : uint8_t {
	faithful = 0, nouveau = 1, dummycabooseval = 2
};
enum class projection_mode : uint8_t {
	globe_orthographic = 0,
	rectangle = 1,
	globe_perspective = 2,
	square = 3,
	num_of_modes = 4
};
}
