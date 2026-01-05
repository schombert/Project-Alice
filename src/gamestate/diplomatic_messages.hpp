#pragma once

#include "dcon_generated_ids.hpp"
#include "system_state_forward.hpp"
#include "container_types.hpp"
#include "date_interface.hpp"
#include "diplomatic_messages_containers.hpp"

namespace diplomatic_message {

bool can_accept_crisis_offer(sys::state& state, dcon::nation_id from, dcon::nation_id to, sys::full_wg offer);
bool can_accept_crisis_peace_offer(sys::state& state, dcon::nation_id from, dcon::nation_id to, dcon::peace_offer_id peace);

bool ai_will_accept(sys::state& state, message const& m);

void decline(sys::state& state, message const& m);
void accept(sys::state& state, message const& m);
bool can_accept(sys::state& state, message const& m);

void post(sys::state& state, message const& m);
void update_pending(sys::state& state);

} // namespace diplomatic_message
