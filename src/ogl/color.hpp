#pragma once

#include "system_state.hpp"
#include <cstdint>

namespace ogl {

// borrowed from http://www.burtleburtle.net/bob/hash/doobs.html
inline constexpr uint32_t color_from_hash(uint32_t color) {
	uint32_t m1 = 0x1337CAFE;
	uint32_t m2 = 0xDEADBEEF;
	m1 -= m2; m1 -= color; m1 ^= (color >> 13);
	m2 -= color; m2 -= m1; m2 ^= (m1 << 8);
	color -= m1; color -= m2; color ^= (m2 >> 13);
	m1 -= m2; m1 -= color; m1 ^= (color >> 12);
	m2 -= color; m2 -= m1; m2 ^= (m1 << 16);
	color -= m1; color -= m2; color ^= (m2 >> 5);
	m1 -= m2; m1 -= color; m1 ^= (color >> 3);
	m2 -= color; m2 -= m1; m2 ^= (m1 << 10);
	color -= m1; color -= m2; color ^= (m2 >> 15);
	return color;
}

uint32_t get_ui_color(sys::state& state, dcon::issue_option_id id){
	return ogl::color_from_hash(uint32_t(id.index()));
}

template<class T>
uint32_t get_ui_color(sys::state& state, T id) {
	return dcon::fatten(state.world, id).get_color();
}

}
