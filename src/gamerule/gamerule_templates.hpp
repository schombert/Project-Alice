#include "dcon_generated_ids.hpp"
#include "system_state.hpp"
#pragma once

namespace gamerule {


template<typename enum_type> requires std::is_enum<enum_type>::value
enum_type get_gamerule_setting(sys::state& state, dcon::gamerule_id gamerule) {
	assert(gamerule);
	return static_cast<enum_type>(state.world.gamerule_get_current_setting(gamerule));
}



}

