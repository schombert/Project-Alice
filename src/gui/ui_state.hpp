#pragma once

#include <chrono>
#include "gui_units.hpp"
#include "opengl_wrapper_containers.hpp"
#include "SPSCQueue.h"

namespace ui {
typedef void(*ui_function)(sys::state& state);

struct state {
	element_base* under_mouse = nullptr;
	element_base* left_mouse_hold_target = nullptr;
	uint16_t scrollbar_timer = 0;
	uint16_t fps_timer = 0;
	std::chrono::time_point<std::chrono::steady_clock> last_render_time{};
	std::chrono::microseconds time_since_last_render{ };
	bool scrollbar_continuous_movement = false;
	float last_fps = 0.f;
	bool lazy_load_in_game = false;
	element_base* scroll_target = nullptr;
	element_base* drag_target = nullptr;
	element_base* edit_target_internal = nullptr;
	element_base* last_tooltip = nullptr;
	element_base* mouse_sensitive_target = nullptr;
	xy_pair target_ul_bounds = xy_pair{ 0, 0 };
	xy_pair target_lr_bounds = xy_pair{ 0, 0 };
	int32_t last_tooltip_sub_index = -1;
	uint32_t cursor_size = 16;
	int32_t target_distance = 0;
	edit_selection_mode selecting_edit_text = edit_selection_mode::none;

	xy_pair relative_mouse_location = xy_pair{ 0, 0 };
	std::unique_ptr<element_base> units_root;

	std::unique_ptr<element_base> rgos_root;
	std::unique_ptr<element_base> province_details_root;
	std::unique_ptr<element_base> root;
	std::unique_ptr<element_base> military_root;
	std::unique_ptr<element_base> nation_picker;
	std::unique_ptr<element_base> end_screen;
	std::unique_ptr<element_base> select_states_legend;
	std::unique_ptr<element_base> select_national_identity_root;
	std::unique_ptr<element_base> army_group_selector_root;
	std::unique_ptr<element_base> army_group_deselector_root;
	std::unique_ptr<element_base> economy_viewer_root;
	std::unique_ptr<element_base> root_production_view;

	template<typename F>
	void for_each_root(F&& func) {
		if(rgos_root)
			func(*rgos_root);
		if(province_details_root)
			func(*province_details_root);
		if(root)
			func(*root);
		if(military_root)
			func(*military_root);
		if(nation_picker)
			func(*nation_picker);
		if(select_national_identity_root)
			func(*select_national_identity_root);
		if(army_group_selector_root)
			func(*army_group_selector_root);
		if(army_group_deselector_root)
			func(*army_group_deselector_root);
		if(economy_viewer_root)
			func(*economy_viewer_root);
		if(select_states_legend)
			func(*select_states_legend);
		if(root_production_view)
			func(*root_production_view);
	}

	std::unique_ptr<tool_tip> tooltip;
	std::unique_ptr<grid_box> unit_details_box;
	alice_ui::pop_up_menu_container* popup_menu = nullptr;
	ankerl::unordered_dense::map<dcon::text_key, element_target, hash_text_key> defs_by_name;
	ankerl::unordered_dense::map<dcon::text_key, dcon::gfx_object_id, hash_text_key> gfx_by_name;
	ankerl::unordered_dense::map<std::string, sys::aui_pending_bytes> new_ui_windows;
	std::vector<simple_fs::file> held_open_ui_files;

	std::vector<uint32_t> rebel_flags;

	// elements we are keeping track of
	element_base* fps_counter = nullptr;
	element_base* console_window = nullptr; // console window
	element_base* console_window_r = nullptr;
	element_base* topbar_window = nullptr;
	element_base* topbar_subwindow = nullptr; // current tab window
	element_base* province_window = nullptr;
	element_base* search_window = nullptr;
	element_base* ledger_window = nullptr;
	element_base* r_ledger_window = nullptr; // end screen ledger window
	element_base* diplomacy_subwindow = nullptr;
	element_base* politics_subwindow = nullptr;
	element_base* population_subwindow = nullptr;
	element_base* production_subwindow = nullptr;
	element_base* build_new_factory_subwindow = nullptr;
	element_base* trade_subwindow = nullptr;
	element_base* unit_window_army = nullptr;
	element_base* unit_window_navy = nullptr;
	element_base* build_unit_window = nullptr;
	element_base* msg_filters_window = nullptr;
	element_base* outliner_window = nullptr;
	element_base* technology_subwindow = nullptr;
	element_base* military_subwindow = nullptr;
	element_base* request_window = nullptr;
	unit_details_window<dcon::army_id>* army_status_window = nullptr;
	unit_details_window<dcon::navy_id>* navy_status_window = nullptr;
	element_base* army_group_window_land = nullptr;
	element_base* multi_unit_selection_window = nullptr;
	element_base* msg_log_window = nullptr;
	element_base* msg_window = nullptr;
	element_base* menubar_window = nullptr;
	element_base* main_menu_win = nullptr; // The actual main menu
	element_base* chat_window = nullptr;
	element_base* r_chat_window = nullptr;
	element_base* map_gradient_legend = nullptr;
	element_base* map_civ_level_legend = nullptr;
	element_base* map_col_legend = nullptr;
	element_base* map_dip_legend = nullptr;
	element_base* map_rr_legend = nullptr;
	element_base* map_nav_legend = nullptr;
	element_base* map_rank_legend = nullptr;
	element_base* map_rec_legend = nullptr;
	element_base* tl_chat_list = nullptr;
	element_base* error_win = nullptr;
	element_base* naval_combat_window = nullptr;
	element_base* army_combat_window = nullptr;
	element_base* change_leader_window = nullptr;
	element_base* macro_builder_window = nullptr;
	element_base* request_topbar_listbox = nullptr;
	element_base* build_province_unit_window = nullptr;
	element_base* disband_unit_window = nullptr;
	element_base* gamerules_window = nullptr;

	context_menu_window* context_menu = nullptr;
	factory_refit_window* factory_refit_win = nullptr;

	std::array<chat_message, 32> chat_messages;
	std::vector<dcon::technology_id> tech_queue;
	uint8_t chat_messages_index = 0;

	dcon::gfx_object_id bg_gfx_id{};
	dcon::gfx_object_id load_screens_gfx_id[8];

	std::vector<std::unique_ptr<element_base>> endof_landcombat_windows;
	std::vector<std::unique_ptr<element_base>> endof_navalcombat_windows;

	ogl::captured_element drag_and_drop_image;
	std::any current_drag_and_drop_data;
	ui::drag_and_drop_data current_drag_and_drop_data_type = ui::drag_and_drop_data::none;

	int32_t held_game_speed = 1; // used to keep track of speed while paused
	int32_t current_template = -1; // used as the currently edited template
	std::vector<sys::macro_builder_template> templates;
	uint16_t tooltip_font = 0;
	uint16_t default_header_font = 0;
	uint16_t default_body_font = 0;
	bool ctrl_held_down = false;
	bool shift_held_down = false;
	military::special_army_order selected_army_order;
	ankerl::unordered_dense::map<dcon::gamerule_id, uint8_t, sys::gamerule_hash> gamerule_ui_settings;
	rigtorp::SPSCQueue<ui_function> queued_invocations;
	bool recently_pressed_resync = false; // flag to prevent clicking the resync button again after already pressing it once

	float last_tick_investment_pool_change;

	void set_mouse_sensitive_target(sys::state& state, element_base* target);
	void set_focus_target(sys::state& state, element_base* target);
	void update_timing();
	void update_scroll(sys::state& game_state);
	void update_tooltip(sys::state& state, ui::mouse_probe tooltip_probe, int32_t tooltip_sub_index, int16_t max_height);
	void populate_tooltip(sys::state& state, ui::mouse_probe tooltip_probe, int32_t tooltip_sub_index, int16_t max_height);
	void reposition_tooltip(ui::urect tooltip_bounds, int16_t root_height, int16_t root_width);
	void handle_map_tooltip(sys::state& state, int16_t max_height);
	void render_tooltip(sys::state& state, bool follow_mouse, int32_t mouse_x, int32_t mouse_y, int32_t screen_size_x, int32_t screen_size_y, float ui_scale);
	void invoke_on_ui_thread(ui_function func); // Invoke a function on the UI thread, using a SPSC queue

	state();
	~state();
};

}
