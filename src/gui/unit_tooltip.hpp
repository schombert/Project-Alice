#pragma once

#include "system_state.hpp"

namespace ui {

struct unitamounts {
	uint16_t type1 = 0;
	uint32_t type1_pop = 0;

	uint16_t type2 = 0;
	uint32_t type2_pop = 0;

	uint16_t type3 = 0;
	uint32_t type3_pop = 0;
};

unitamounts calc_amounts_from_army(sys::state& state, dcon::army_fat_id army);
unitamounts calc_amounts_from_navy(sys::state& state, dcon::navy_fat_id navy);

void populate_unit_tooltip(sys::state& state, text::columnar_layout& contents, dcon::province_id prov);
void single_unit_tooltip(sys::state& state, text::columnar_layout& contents, dcon::army_id a);
void single_unit_tooltip(sys::state& state, text::columnar_layout& contents, dcon::navy_id n);

}
