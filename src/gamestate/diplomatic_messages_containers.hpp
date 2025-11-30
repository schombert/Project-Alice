#pragma once

#include "dcon_generated_ids.hpp"
#include "date_interface.hpp"

namespace diplomatic_message {

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
	state_transfer = 9,
	free_trade_agreement = 10
};

/// <summary>
/// Holds data regarding a diplomatic message between two specified nations at a certain date, whether it be an alliance request, peace offer, calling an ally, etc. Also contains other optional data such as a pertinent war, crisis, state, etc.
/// </summary>
struct message {
	union dtype {
		dcon::war_id war; //2
		dcon::peace_offer_id peace; //2
		sys::full_wg crisis_offer; //10
		dcon::state_definition_id state; //2
		dtype() {
			memset(this, 0, sizeof(*this));
		}
	} data; //10
	sys::date when; //2
	dcon::nation_id from; //2
	dcon::nation_id to; //2
	type_t type = diplomatic_message::type_t::none; //1
	bool automatic_call = false; // This was 1 byte padding previously, now used for checking if war call-to-arms were done automatically (by attacker by having the "call all allies" btn checked, or defender allies when they are automatically called when declared on

	bool operator==(const message& other) const {
		// Comparing the unused members of a union is technically UB.
		// so it is copied into a buffer first before comparing them
		// there is a very small chance that two seperate unions can be equal even though they hold diffrent types.
		char data_buffer[sizeof(data)];
		std::memcpy(data_buffer, &data, sizeof(data));

		char data_buffer_other[sizeof(data)];
		std::memcpy(data_buffer_other, &other.data, sizeof(data));

		return std::memcmp(data_buffer, data_buffer_other, sizeof(data)) == 0 && when == other.when && from == other.from && to == other.to && type == other.type && automatic_call == other.automatic_call;

	}
	bool operator!=(const message& other) const {
		return !(*this == other);
	}


	message() : type(diplomatic_message::type_t::none) {
	}
};
static_assert(sizeof(message) ==
	sizeof(message::from)
	+ sizeof(message::to)
	+ sizeof(message::when)
	+ sizeof(message::data)
	+ sizeof(message::type)
	+ sizeof(message::automatic_call));

using type = type_t;
}
