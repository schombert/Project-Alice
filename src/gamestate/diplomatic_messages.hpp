#pragma once

#include "dcon_generated.hpp"
#include "container_types.hpp"

namespace diplomatic_message {

enum class type : uint8_t {
	none = 0,
	access_request = 1,
	alliance_request = 2,
	call_ally_request = 3
};

struct message {
	dcon::nation_id from;
	dcon::nation_id to;
	sys::date when;
	union dtype {
		dcon::war_id war;

		dtype() { }
	} data;
	type type;

	message() : type(diplomatic_message::type::none) { }
};

void decline_message(sys::state& state, message const& m);
void accept_message(sys::state& state, message const& m);

void post_message(sys::state& state, message const& m);
void update_pending_messages(sys::state& state);

}
