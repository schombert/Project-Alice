#pragma once

#include <string>
#include "gui_element_types.hpp"
#include "system_state_forward.hpp"
#include "dcon_generated_ids.hpp"

namespace ui {

enum class unit_sort_type : uint8_t {
	name,
	strength
};

struct unit_sort_data {
	unit_sort_type sort_by = unit_sort_type::name;
	bool ascending = false;
};

template<class T>
class military_units_window : public window_element_base {
private:
	image_element_base* cdts_icon = nullptr;
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;

	unit_sort_data unit_sort{};
};

using military_units_window_army = military_units_window<dcon::army_id>;
using military_units_window_navy = military_units_window<dcon::navy_id>;
}
