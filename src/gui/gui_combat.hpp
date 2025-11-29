#pragma once

#include "gui_element_types.hpp"
#include "system_state_forward.hpp"
#include "military.hpp"

namespace ui {
class land_combat_end_popup : public window_element_base {
public:
	military::land_battle_report report;
	static std::vector<std::unique_ptr<ui::land_combat_end_popup>> land_reports_pool;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
	static void make_new_report(sys::state& state, military::land_battle_report const& r);
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
};

class land_combat_window : public window_element_base {
public:
	dcon::land_battle_id battle;
	void on_create(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
};

class naval_combat_end_popup : public window_element_base {
public:
	military::naval_battle_report report;
	static std::vector<std::unique_ptr<ui::naval_combat_end_popup>> naval_reports_pool;


	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
	static void make_new_report(sys::state& state, military::naval_battle_report const& r);
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
};

class naval_combat_window : public window_element_base {
public:
	dcon::naval_battle_id battle;
	void on_create(sys::state& state) noexcept override;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	void on_update(sys::state& state) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
};

}
