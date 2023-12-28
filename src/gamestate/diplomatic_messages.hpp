#pragma once

#include "dcon_generated.hpp"
#include "container_types.hpp"

namespace diplomatic_message {

constexpr inline int32_t expiration_in_days = 15;

enum class type_t : uint8_t {
	none = 0,
	access_request = 1,
	alliance_request = 2,
	call_ally_request = 3,
	be_crisis_primary_defender = 4,
	be_crisis_primary_attacker = 5,
	peace_offer = 6,
	take_crisis_side_offer = 7,
	crisis_peace_offer = 8,
	state_transfer = 9
};

struct message {
	dcon::nation_id from; //2
	dcon::nation_id to; //2
	sys::date when; //2
	union dtype {
		dcon::war_id war; //2
		dcon::peace_offer_id peace; //2
		sys::crisis_join_offer crisis_offer; //10
		dcon::state_definition_id state; //2

		dtype() { }
	} data;
	type_t type = diplomatic_message::type_t::none; //1
	uint8_t padding = 0; //1

	message() : type(diplomatic_message::type_t::none) { }
};
static_asset(sizeof(message) ==
	sizeof(message::from)
	+ sizeof(message::to)
	+ sizeof(message::when)
	+ sizeof(message::data)
	+ sizeof(message::type)
	+ sizeof(message::padding))

using type = type_t;

bool can_accept_crisis_offer(sys::state& state, dcon::nation_id from, dcon::nation_id to, sys::crisis_join_offer const& offer);
bool can_accept_crisis_peace_offer(sys::state& state, dcon::nation_id from, dcon::nation_id to, dcon::peace_offer_id peace);

bool ai_will_accept(sys::state& state, message const& m);

void decline(sys::state& state, message const& m);
void accept(sys::state& state, message const& m);

void post(sys::state& state, message const& m);
void update_pending(sys::state& state);

} // namespace diplomatic_message
