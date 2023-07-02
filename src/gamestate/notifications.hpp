#pragma once

#include "container_types.hpp"
#include "text.hpp"

namespace notification {

struct message {
	std::function<void(sys::state&, text::layout_base&)> body;
	char const* title = nullptr;
	dcon::nation_id about;	 // which nation this message is primarily about (i.e. who the thing happened to)
	sys::message_setting_type type;
};

void post(sys::state& state, message&& m);
bool nation_is_interesting(sys::state& state, dcon::nation_id n);

} // namespace notification
