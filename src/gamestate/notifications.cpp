#include "notifications.hpp"
#include "system_state.hpp"

namespace notification {

void post(sys::state& state, message&& m) {
	//
	// TODO: pre filter out any messages that the player is not interested in at all according to their message settings.
	// But, probably don't try to fiter out by "interesting" except for considering whether the player nation is itself involved
	// as that will probably be a more computationally expensive check
	//

	// A burst of messages can fill the bounded queue. Dropping the newest
	// notification is preferable to terminating the game in debug builds.
	(void)state.new_messages.try_emplace(std::move(m));
}

bool nation_is_interesting(sys::state& state, dcon::nation_id n) {
	return state.world.nation_get_is_interesting(n);
}

} // namespace notification
