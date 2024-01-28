#pragma once
#include "text.hpp"
#include "system_state.hpp"

namespace ui {

void display_leader_attributes(sys::state& state, dcon::leader_id lid, text::layout_base& contents, int32_t indent = 0);
void display_leader_full(sys::state& state, dcon::leader_id lid, text::layout_base& contents, int32_t indent = 0);

}
