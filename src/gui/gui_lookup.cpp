#include "system_state.hpp"
#include "dcon_generated_ids.hpp"

namespace ui {
dcon::gui_def_id get_gui_definition_id(sys::state& state, std::string_view name) {
	auto it = state.ui_state.defs_by_name.find(state.lookup_key(name));
	if(it == state.ui_state.defs_by_name.end()) return { };

	return it->second.definition;
}
element_data const& get_element_data(sys::state& state, dcon::gui_def_id id) {
	return state.ui_defs.gui[id];
}
}
