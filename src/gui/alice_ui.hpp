#pragma once
#include "gui_element_base.hpp"
#include "system_state.hpp"

namespace alice_ui {
std::unique_ptr<ui::element_base> make_macrobuilder2_main(sys::state& state);
}
