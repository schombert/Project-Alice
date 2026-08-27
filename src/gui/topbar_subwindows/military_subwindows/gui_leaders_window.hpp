#pragma once

#include <string_view>
#include "gui_element_types.hpp"
#include "system_state_forward.hpp"
#include "dcon_generated_ids.hpp"

namespace ui {
class leaders_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
};
}
