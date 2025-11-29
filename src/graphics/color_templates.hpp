#pragma once
#include "system_state.hpp"

namespace ogl {
template<class T>
inline uint32_t get_ui_color(sys::state& state, T id) {
	return dcon::fatten(state.world, id).get_color();
}
template<>
inline uint32_t get_ui_color(sys::state& state, dcon::political_party_id id) {
	return dcon::fatten(state.world, id).get_ideology().get_color();
}
template<>
inline uint32_t get_ui_color(sys::state& state, dcon::issue_option_id id) {
	return ogl::color_from_hash(uint32_t(id.index()));
}
template<>
inline uint32_t get_ui_color(sys::state& state, dcon::crime_id id) {
	return ogl::color_from_hash(uint32_t(id.index()));
}
}
