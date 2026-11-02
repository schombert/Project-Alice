#pragma once

#include <string_view>
#include "gui_element_types.hpp"
#include "system_state_forward.hpp"
#include "dcon_generated_ids.hpp"



namespace ui {

enum class leader_sort_type : uint8_t {
	prestige,
	type,
	name,
	assignment
};

struct leader_sort_data {
	leader_sort_type sort_by = leader_sort_type::name;
	bool ascending = false;
};

class leaders_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;

	leader_sort_data leader_sort{};

};
}
