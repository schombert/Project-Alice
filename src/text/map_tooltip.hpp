#pragma once 

#include "gui_element_types.hpp"


namespace text {

void populate_map_tooltip(sys::state& state, int16_t x, int16_t y, text::columnar_layout& contents, dcon::province_id prov);

}
