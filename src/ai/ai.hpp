#pragma once
#include "dcon_generated.hpp"
#include "container_types.hpp"
#include "text.hpp"

namespace ai {

void update_ai_general_status(sys::state& state);
void form_alliances(sys::state& state);
bool ai_will_accept_alliance(sys::state& state, dcon::nation_id target, dcon::nation_id from);
void explain_ai_alliance_reasons(sys::state& state, dcon::nation_id target, text::layout_base& contents, int32_t indent);
bool ai_will_grant_access(sys::state& state, dcon::nation_id target, dcon::nation_id from);
void explain_ai_access_reasons(sys::state& state, dcon::nation_id target, text::layout_base& contents, int32_t indent);
void update_ai_research(sys::state& state);
void initialize_ai_tech_weights(sys::state& state);
void update_influence_priorities(sys::state& state);
void perform_influence_actions(sys::state& state);
void update_focuses(sys::state& state);
void identify_focuses(sys::state& state);
}
