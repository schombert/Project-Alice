#include "system_state.hpp"
#include "gui_element_base.hpp"
#include "gui_element_types.hpp"
#include "gui_deserialize.hpp"
#include "alice_ui.hpp"
#include "demographics.hpp"

#include "macrobuilder2.cpp"
#include "budgetwindow.cpp"
#include "demographicswindow.cpp"

namespace alice_ui {

bool pop_passes_filter(sys::state& state, dcon::pop_id p) {
	if(popwindow::excluded_cultures.contains(state.world.pop_get_culture(p).id.index()))
		return false;
	if(popwindow::excluded_types.contains(state.world.pop_get_poptype(p).id.index()))
		return false;
	if(popwindow::excluded_religions.contains(state.world.pop_get_religion(p).id.index()))
		return false;
	if(popwindow::excluded_states.contains(state.world.province_get_state_membership(state.world.pop_get_province_from_pop_location(p)).id.index()))
		return false;
	if(!popwindow::show_colonial && state.world.province_get_is_colonial(state.world.pop_get_province_from_pop_location(p)))
		return false;
	if(!popwindow::show_non_colonial && !state.world.province_get_is_colonial(state.world.pop_get_province_from_pop_location(p)))
		return false;
	return true;
}

}
