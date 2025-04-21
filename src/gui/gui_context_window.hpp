#pragma once
#include "gui_element_types.hpp"

namespace ui {

struct context_menu_context {
	dcon::province_id province;
	dcon::factory_id factory;
};

void hide_context_menu(sys::state& state);
void show_context_menu(sys::state& state, context_menu_context context);

}
