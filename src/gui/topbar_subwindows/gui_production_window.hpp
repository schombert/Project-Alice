#pragma once

#include "gui_element_types.hpp"

namespace ui {

class production_state_listbox;
class production_state_invest_listbox;
class production_foreign_investment_window;

class province_build_new_factory : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override;
	void button_action(sys::state& state) noexcept override;
	tooltip_behavior has_tooltip(sys::state& state) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
};

class production_window : public generic_tabbed_window<production_window_tab> {
	bool show_empty_states = true;
	std::unique_ptr<bool[]> show_output_commodity;

	production_state_listbox* state_listbox = nullptr;
	production_state_invest_listbox* state_listbox_invest = nullptr;
	element_base* nf_win = nullptr;
	element_base* project_window = nullptr;
	production_foreign_investment_window* foreign_invest_win = nullptr;

	dcon::factory_id selected_factory;
	factory_refit_window* factory_refit_win = nullptr;

	sys::commodity_group curr_commodity_group{};
	dcon::province_id focus_province{};
	dcon::nation_id foreign_nation{};
	xy_pair base_commodity_offset{33, 50};
	xy_pair commodity_offset{33, 50};

	std::vector<element_base*> factory_elements;
	std::vector<element_base*> investment_brow_elements;
	std::vector<element_base*> project_elements;
	std::vector<element_base*> good_elements;
	std::vector<element_base*> investment_nation;
	std::vector<bool> commodity_filters;
	bool open_foreign_invest = false;

	void set_visible_vector_elements(sys::state& state, std::vector<element_base*>& elements, bool v) noexcept;
	void hide_sub_windows(sys::state& state) noexcept;
public:
	void on_create(sys::state& state) noexcept override;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
};

void open_foreign_investment(sys::state& state, dcon::nation_id n);
void open_build_foreign_factory(sys::state& state, dcon::province_id st);

} // namespace ui
