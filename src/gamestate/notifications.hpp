#pragma once

#include "container_types.hpp"
#include "text.hpp"

namespace notification {

struct message {
	std::function<void(sys::state&, text::layout_base&)> body;
	char const* title = nullptr;
	dcon::nation_id source;	 // which nation caused the notification to be sent
	dcon::nation_id target;	 // which nation is primarily affected by the event (if != source)
	dcon::nation_id third;	 // a secondary nation affected by the event
	sys::message_base_type type;
};

void post(sys::state& state, message&& m);
bool nation_is_interesting(sys::state& state, dcon::nation_id n);

} // namespace notification
