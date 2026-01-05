#pragma once

#include <string_view>
#include "gui_element_types.hpp"
#include "system_state_forward.hpp"
#include "dcon_generated_ids.hpp"

namespace ui {

class units_build_listbox;
class units_queue_listbox;

class build_unit_large_window : public window_element_base {
	dcon::unit_type_id unit_type{};
	dcon::unit_type_id first_land_type{};
	dcon::unit_type_id first_naval_type{};

public:
	void on_create(sys::state& state) noexcept override;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	void set_army_visible(sys::state& state);
	void set_army_invisible(sys::state& state);
	void set_navy_visible(sys::state& state);
	void set_navy_invisible(sys::state& state);
	message_result set(sys::state& state, Cyto::Any& payload) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
	std::vector<element_base*> army_elements;
	std::vector<element_base*> navy_elements;
	ui::units_build_listbox* units_listbox = nullptr;
	ui::units_queue_listbox* units_queue = nullptr;
};

class units_province_build_listbox;
class units_province_queue_listbox;
class build_unit_province_cancel_all_units_button;

class build_unit_province_window : public window_element_base {
	std::vector<element_base*> army_elements;
	std::vector<element_base*> navy_elements;
	units_province_build_listbox* units_listbox = nullptr;
	units_province_queue_listbox* units_queue = nullptr;
	build_unit_province_cancel_all_units_button* cancel_all_btn = nullptr;
public:
	void on_create(sys::state& state) noexcept override;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
};

}
