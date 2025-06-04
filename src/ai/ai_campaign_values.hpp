#pragma once
#include "dcon_generated.hpp"

namespace sys {
struct state;
}

namespace text {
struct layout_base;
}

namespace ai {

int32_t future_rebels_in_nation(sys::state& state, dcon::nation_id n);
int16_t calculate_desired_army_size(sys::state& state, dcon::nation_id nation);
float estimate_naval_strength(sys::state& state, dcon::nation_id n);

float estimate_strength(sys::state& state, dcon::nation_id n);
float estimate_defensive_strength(sys::state& state, dcon::nation_id n);
float estimate_additional_offensive_strength(sys::state& state, dcon::nation_id n, dcon::nation_id target);

bool does_have_naval_supremacy(sys::state& state, dcon::nation_id n, dcon::nation_id target);

bool ai_will_accept_free_trade(sys::state& state, dcon::nation_id target, dcon::nation_id from);
void explain_ai_trade_agreement_reasons(sys::state& state, dcon::nation_id target, text::layout_base& contents, int32_t indent);

bool ai_will_grant_access(sys::state& state, dcon::nation_id target, dcon::nation_id from);
void explain_ai_access_reasons(sys::state& state, dcon::nation_id target, text::layout_base& contents, int32_t indent);
}
