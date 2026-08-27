#include <string_view>
#include "system_state_forward.hpp"

namespace dcon {
class gui_def_id;
}

namespace ui {
dcon::gui_def_id get_gui_definition_id(sys::state& state, std::string_view name);
element_data const& get_element_data(sys::state& state, dcon::gui_def_id id);
}
