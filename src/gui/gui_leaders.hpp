#pragma once
#include "dcon_generated_ids.hpp"
#include "commands.hpp"
#include "gui_common_elements.hpp"
#include "prng.hpp"
#include "text.hpp"

namespace ui {

template<typename T>
class leader_prestige_progress_bar : public vertical_progress_bar {
public:
	void on_update(sys::state& state) noexcept override;
	void on_create(sys::state& state) noexcept override;
	tooltip_behavior has_tooltip(sys::state& state) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	dcon::leader_id get_leader_id(sys::state& state) noexcept;
};

using leader_prestige_progress_bar_army = leader_prestige_progress_bar<dcon::army_id>;
using leader_prestige_progress_bar_navy = leader_prestige_progress_bar<dcon::navy_id>;
using leader_prestige_progress_bar_leader = leader_prestige_progress_bar<dcon::leader_id>;

class leader_select_listbox;

class leader_selection_window : public window_element_base {
public:
	dcon::army_id a;
	dcon::navy_id v;
	leader_select_listbox* lb = nullptr;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
};

void open_leader_selection(sys::state& state, dcon::army_id a, dcon::navy_id v, int32_t x, int32_t y);

}
