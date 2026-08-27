#pragma once 

#include "gui_element_types.hpp"


namespace ui {

void populate_map_tooltip(sys::state& state, text::columnar_layout& contents, dcon::province_id prov);

constexpr uint32_t max_units_in_province_tooltip = 10;

}
