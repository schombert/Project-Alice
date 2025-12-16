#pragma once

#include "system_state_forward.hpp"
#include "dcon_generated_ids.hpp"
#include "gui_element_types.hpp"
#include "gui_element_base.hpp"

namespace ui {
enum class outline_color : uint8_t {
	gray = 0, gold = 1, blue = 2, cyan = 3, red = 4
};
using unit_var = std::variant<std::monostate, dcon::army_id, dcon::navy_id>;

int32_t status_frame(sys::state& state, dcon::army_id a);
int32_t status_frame(sys::state& state, dcon::navy_id a);

class army_group_details_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
};
class army_management_window : public window_element_base {
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
};
class select_army_group_button : public button_element_base {
	void button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
class battleplanner_control : public window_element_base {
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
};
class battleplanner_selection_control : public window_element_base {
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
};
class grid_box : public window_element_base {
public:
	bool as_port = false;
	dcon::province_id for_province;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
	void open(sys::state& state, ui::xy_pair location, ui::xy_pair source_size, dcon::province_id p, bool port);
};

class dug_in_icon;

template<class T>
class unit_selection_panel;

template<class T>
class unit_upgrade_window;

template<class T>
class unit_details_window : public window_element_base {
	simple_text_element_base* unitspeed_text = nullptr;
	image_element_base* unitrecon_icon = nullptr;
	simple_text_element_base* unitrecon_text = nullptr;
	image_element_base* unitengineer_icon = nullptr;
	simple_text_element_base* unitengineer_text = nullptr;
	progress_bar* unitsupply_bar = nullptr;
	dug_in_icon* unitdugin_icon = nullptr;
	unit_selection_panel<T>* unit_selection_win = nullptr;
	unit_upgrade_window<T>* unit_upgrade_win = nullptr;
public:
	T unit_id;
	void on_create(sys::state& state) noexcept override;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	void on_update(sys::state& state) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
};

using army_details_window = unit_details_window<dcon::army_id>;
using navy_details_window = unit_details_window<dcon::navy_id>;

class mulit_unit_selection_panel : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
};


class disband_unit_confirmation : public window_element_base {
	std::vector<dcon::army_id> armies;
	std::vector<dcon::navy_id> navies;
public:
	void on_create(sys::state& state) noexcept override;
	void set_units_to_be_disbanded(sys::state& state, const std::vector<dcon::army_id>& units_to_be_deleted);
	void set_units_to_be_disbanded(sys::state& state, const std::vector<dcon::navy_id>& units_to_be_deleted);
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
};

}
