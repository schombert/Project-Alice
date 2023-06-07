#pragma once

#include "container_types.hpp"
#include "text.hpp"

namespace notification {

struct message {
	std::function<void(sys::state&, text::layout_base&)> title;
	std::function<void(sys::state&, text::layout_base&)> body;
	dcon::nation_id primary;   // which nation this message is primarily about (i.e. who the thing happened to)
	dcon::nation_id secondary; // optional; another nation that caused the thing to happen (for example, in a notification about a new
							   // war, the nation that declared the war, while the primary would be the target of the war).
	sys::message_setting_type type;
};

void post(sys::state& state, message&& m);

} // namespace notification
