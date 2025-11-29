#pragma once

#include <variant>

#include "system_state_forward.hpp"
#include "text.hpp"
#include "events.hpp"

namespace ui {

using event_data_wrapper =  std::variant< event::pending_human_n_event, event::pending_human_f_n_event, event::pending_human_p_event, event::pending_human_f_p_event>;
void populate_event_submap(sys::state& state, text::substitution_map& sub, std::variant<event::pending_human_n_event, event::pending_human_f_n_event, event::pending_human_p_event, event::pending_human_f_p_event> const& phe) noexcept;
void close_expired_event_windows(sys::state& state);
void clear_event_windows(sys::state& state);
bool events_pause_test(sys::state& state);
void new_event_window(sys::state& state, event_data_wrapper dat);

} // namespace ui
