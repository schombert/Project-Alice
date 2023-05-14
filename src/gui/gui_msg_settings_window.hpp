#pragma once

#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"

namespace ui {

class msg_settings_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		//if(name == "close") {
		//    return make_element_by_type<generic_close_button>(state, id);
		//} else {
			return nullptr;
		//}
	}
};

}
