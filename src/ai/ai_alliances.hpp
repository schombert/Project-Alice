#pragma once
#include "dcon_generated.hpp"
namespace sys{
struct state;
}
namespace text {
struct layout_base;
}

namespace ai {

bool ai_will_accept_alliance(sys::state& state, dcon::nation_id target, dcon::nation_id from);
void explain_ai_alliance_reasons(sys::state& state, dcon::nation_id target, text::layout_base& contents, int32_t indent);

void form_alliances(sys::state& state);
void prune_alliances(sys::state& state);

}
