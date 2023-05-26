#prgama once

#include "gui_element_types.hpp"
#include "gui_multiplayer_suchen.hpp"
#include "gui_mainmenu_panel.hpp"

namespace ui {

class country_selection_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "background") {
			return make_element_by_type<button_element_base>(state, id);

		} else if(name == "frontend_backdrop") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "frontend_toptile1") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "frontend_toptile2") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "frontend_toptile3") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "frontend_toptile4") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "frontend_toptile5") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "frontend_bottomtile1") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "frontend_bottomtile2") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "frontend_bottomtile3") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "frontend_bottomtile4") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "frontend_bottomtile5") {
			return make_element_by_type<image_element_base>(state, id);

		} else if(name == "version_label") {
			return make_element_by_type<simple_text_element_base>(state, id);	// TODO - we want to display the current version of PA here

		} else if(name == "multiplayer_menu") {
			return make_element_by_type<multiplayer_menu_window>(state, id);

		} else if(name == "multiplayer_connect_menu") {
			return make_element_by_type<multiplayer_connect_window>(state, id);

		} else if(name == "mainmenu_panel") {
			return make_element_by_type<mainmenu_panel>(state, id);

		} else if(name == "tutorial_panel") {
			return make_element_by_type<tutorial_panel>(state, id);

		} else {
			return nullptr;
		}
	}
};

}
