#pragma once

#include "system_state_forward.hpp"
#include "gui_element_base.hpp"
#include "dcon_generated_ids.hpp"
#include "gui_element_types.hpp"

namespace ui {
enum class reorg_win_action : uint8_t {
	close = 0,
	balance = 1,
};
std::unique_ptr<window_element_base> make_regiment_reorg_window(sys::state& state, dcon::gui_def_id def);
std::unique_ptr<window_element_base> make_ship_reorg_window(sys::state& state, dcon::gui_def_id def);
std::unique_ptr<element_base> make_navy_details_listbox(sys::state& state, dcon::gui_def_id def);
std::unique_ptr<element_base> make_army_details_listbox(sys::state& state, dcon::gui_def_id def);
}
