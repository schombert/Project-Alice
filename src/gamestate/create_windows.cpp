#include "create_windows.hpp"
#include "system_state.hpp"
#include "gui_templates.hpp"
#include "gui_console.hpp"
#include "gui_minimap.hpp"
#include "gui_topbar.hpp"
#include "gui_province_window.hpp"
#include "gui_outliner_window.hpp"
#include "gui_event.hpp"
#include "gui_map_icons.hpp"
#include "gui_diplomacy_request.hpp"
#include "gui_message_window.hpp"
#include "gui_combat.hpp"
#include "gui_chat_window.hpp"
#include "gui_national_identity_select.hpp"
#include "gui_error_window.hpp"
#include "gui_leaders.hpp"
#include "gui_units.hpp"
#include "gui_nation_picker.hpp"
#include "gui_end_window.hpp"
#include "gui_map_legend.hpp"
#include "map_tooltip.hpp"
#include "unit_tooltip.hpp"

namespace ui {

// define one-off templated windows here to avoid accidental usage of headers which use templates and could accidentally lead to viral spread of heavy headers

class map_state_select_title : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::produce_simple_string(state, "alice_state_select_title"));
	}
};

class map_state_select_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::state_definition_id>(state, parent);
		set_button_text(state, text::produce_simple_string(state, state.world.state_definition_get_name(content)));
		//auto it = std::find(state.selected_states.begin(), state.selected_states.end(), content);
		//disabled = (it != state.selected_states.end());
	}
	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::state_definition_id>(state, parent);
		state.state_select(content);
	}
};

class map_state_select_entry : public listbox_row_element_base<dcon::state_definition_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "button") {
			return make_element_by_type<map_state_select_button>(state, id);
		} else {
			return nullptr;
		}
	}
};

class map_state_select_listbox : public listbox_element_base<map_state_select_entry, dcon::state_definition_id> {
protected:
	std::string_view get_row_element_name() override {
		return "alice_select_legend_entry";
	}
public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		assert(state.state_selection.has_value());
		row_contents = state.state_selection->selectable_states;
		update(state);
	}
};

class map_state_select_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "legend_title") {
			return make_element_by_type<map_state_select_title>(state, id);
		} else if(name == "selectable") {
			return make_element_by_type<map_state_select_listbox>(state, id);
		} else {
			return nullptr;
		}
	}
};


void create_in_game_windows(sys::state& state) {
	state.ui_state.lazy_load_in_game = true;

	state.ui_state.unit_details_box = ui::make_element_by_type<ui::grid_box>(state, state.ui_state.defs_by_name.find(state.lookup_key("alice_grid_panel"))->second.definition);
	state.ui_state.unit_details_box->set_visible(state, false);

	// dummy ui root for economy explorer
	state.ui_state.economy_viewer_root = std::make_unique<ui::element_base>();

	//
	state.ui_state.select_states_legend = ui::make_element_by_type<ui::map_state_select_window>(state, state.ui_state.defs_by_name.find(state.lookup_key("alice_select_legend_window"))->second.definition);
	// create UI for national identity selector
	{
		auto key = state.lookup_key("alice_select_legend_window");
		auto def = state.ui_state.defs_by_name.find(key)->second.definition;
		auto window = ui::make_element_by_type<ui::map_national_identity_select_window>(state, def);
		state.ui_state.select_national_identity_root->add_child_to_front(std::move(window));
	}
	// create ui for army selector
	{
		{
			auto key = state.lookup_key("alice_armygroup_selection_control_panel");
			auto def = state.ui_state.defs_by_name.find(key)->second.definition;
			auto window = ui::make_element_by_type<ui::battleplanner_selection_control>(state, def);
			state.ui_state.army_group_selector_root->add_child_to_front(std::move(window));
		}
		{
			auto key = state.lookup_key("alice_armygroup_exit_units_selection");
			auto def = state.ui_state.defs_by_name.find(key)->second.definition;
			auto button = ui::make_element_by_type<ui::go_to_battleplanner_button>(state, def);
			state.ui_state.army_group_selector_root->add_child_to_front(std::move(button));
		}
	}

	// create ui for battleplanner
	{
		state.world.for_each_province([&](dcon::province_id id) {
			auto ptr = ui::make_element_by_type<ui::army_group_counter_window>(state, "alice_army_group_on_map");
			static_cast<ui::army_group_counter_window*>(ptr.get())->prov = id;
			state.ui_state.military_root->add_child_to_front(std::move(ptr));
		});

		{
			auto key = state.lookup_key("alice_army_group_regiments_list");
			auto def = state.ui_state.defs_by_name.find(key)->second.definition;
			auto new_elm_army_group = ui::make_element_by_type<ui::army_group_details_window>(state, def);
			state.ui_state.army_group_window_land = static_cast<ui::army_group_details_window*>(new_elm_army_group.get());
			new_elm_army_group->set_visible(state, true);
			state.ui_state.military_root->add_child_to_front(std::move(new_elm_army_group));
		}

		{
			auto key = state.lookup_key("alice_exit_battleplanner");
			auto def = state.ui_state.defs_by_name.find(key)->second.definition;
			auto button = ui::make_element_by_type<ui::go_to_base_game_button>(state, def);
			state.ui_state.military_root->add_child_to_front(std::move(button));
		}

		{
			auto key = state.lookup_key("alice_battleplanner_control_panel");
			auto def = state.ui_state.defs_by_name.find(key)->second.definition;
			auto window = ui::make_element_by_type<ui::battleplanner_control>(state, def);
			state.ui_state.military_root->add_child_to_front(std::move(window));
		}
	}


	state.ui_state.end_screen = std::make_unique<ui::container_base>();
	{
		auto ewin = ui::make_element_by_type<ui::end_window>(state, state.ui_state.defs_by_name.find(state.lookup_key("back_end"))->second.definition);
		state.ui_state.end_screen->add_child_to_front(std::move(ewin));
	}
	{
		auto window = ui::make_element_by_type<ui::console_window>(state, "alice_console_window");
		state.ui_state.console_window = window.get();
		state.ui_state.root->add_child_to_front(std::move(window));
	}
	state.world.for_each_province([&](dcon::province_id id) {
		if(state.world.province_get_port_to(id)) {
			auto ptr = ui::make_element_by_type<ui::port_window>(state, "alice_port_icon");
			static_cast<ui::port_window*>(ptr.get())->set_province(state, id);
			state.ui_state.units_root->add_child_to_front(std::move(ptr));
		}
	});

	state.world.for_each_province([&](dcon::province_id id) {
		auto ptr = ui::make_element_by_type<ui::unit_counter_window>(state, "alice_map_unit");
		static_cast<ui::unit_counter_window*>(ptr.get())->prov = id;
		state.ui_state.units_root->add_child_to_front(std::move(ptr));
	});
	state.world.for_each_province([&](dcon::province_id id) {
		auto ptr = ui::make_element_by_type<ui::rgo_icon>(state, "alice_rgo_mapicon");
		static_cast<ui::rgo_icon*>(ptr.get())->content = id;
		state.ui_state.rgos_root->add_child_to_front(std::move(ptr));
	});
	province::for_each_land_province(state, [&](dcon::province_id id) {
		auto ptr = ui::make_element_by_type<ui::province_details_container>(state, "alice_province_values");
		static_cast<ui::province_details_container*>(ptr.get())->prov = id;
		state.ui_state.province_details_root->add_child_to_front(std::move(ptr));
	});
	{
		auto new_elm = ui::make_compact_chat_messages_list(state, "chat_list");
		new_elm->base_data.position.x += 156; // nudge
		new_elm->base_data.position.y += 24; // nudge
		new_elm->impl_on_update(state);
		state.ui_state.tl_chat_list = new_elm.get();
		state.ui_state.root->add_child_to_back(std::move(new_elm));
	}
	{
		auto new_elm = ui::make_element_by_type<ui::outliner_window>(state, "outliner");
		state.ui_state.outliner_window = new_elm.get();
		new_elm->impl_on_update(state);
		state.ui_state.root->add_child_to_front(std::move(new_elm));
		// Has to be created AFTER the outliner window
		// The topbar has this button within, however since the button isn't properly displayed, it is better to make
		// it into an independent element of it's own, living freely on the UI root so it can be flexibly moved around when
		// the window is resized for example.
		for(size_t i = state.ui_defs.gui.size(); i-- > 0;) {
			auto gdef = dcon::gui_def_id(dcon::gui_def_id::value_base_t(i));
			if(state.to_string_view(state.ui_defs.gui[gdef].name) == "topbar_outlinerbutton_bg") {
				auto new_bg = ui::make_element_by_type<ui::outliner_button>(state, gdef);
				state.ui_state.root->add_child_to_front(std::move(new_bg));
				break;
			}
		}
		// Then create button atop
		for(size_t i = state.ui_defs.gui.size(); i-- > 0;) {
			auto gdef = dcon::gui_def_id(dcon::gui_def_id::value_base_t(i));
			if(state.to_string_view(state.ui_defs.gui[gdef].name) == "topbar_outlinerbutton") {
				auto new_btn = ui::make_element_by_type<ui::outliner_button>(state, gdef);
				new_btn->impl_on_update(state);
				state.ui_state.root->add_child_to_front(std::move(new_btn));
				break;
			}
		}
	}
	{
		auto new_elm = ui::make_element_by_type<ui::minimap_container_window>(state, "alice_menubar");
		state.ui_state.menubar_window = new_elm.get();
		state.ui_state.root->add_child_to_front(std::move(new_elm));
	}
	{
		auto new_elm = ui::make_element_by_type<ui::minimap_picture_window>(state, "minimap_pic");
		state.ui_state.root->add_child_to_front(std::move(new_elm));
	}
	{
		auto new_elm = ui::make_element_by_type<ui::province_view_window>(state, "province_view");
		state.ui_state.root->add_child_to_front(std::move(new_elm));
	}
	{
		auto new_elm_army = ui::make_element_by_type<ui::unit_details_window<dcon::army_id>>(state, "sup_unit_status");
		state.ui_state.army_status_window = static_cast<ui::unit_details_window<dcon::army_id>*>(new_elm_army.get());
		new_elm_army->set_visible(state, false);
		state.ui_state.root->add_child_to_front(std::move(new_elm_army));

		auto new_elm_navy = ui::make_element_by_type<ui::unit_details_window<dcon::navy_id>>(state, "sup_unit_status");
		state.ui_state.navy_status_window = static_cast<ui::unit_details_window<dcon::navy_id>*>(new_elm_navy.get());
		new_elm_navy->set_visible(state, false);
		state.ui_state.root->add_child_to_front(std::move(new_elm_navy));
	}

	{
		auto mselection = ui::make_element_by_type<ui::mulit_unit_selection_panel>(state, "alice_multi_unitpanel");
		state.ui_state.multi_unit_selection_window = mselection.get();
		mselection->set_visible(state, false);
		state.ui_state.root->add_child_to_front(std::move(mselection));
	}
	{
		auto new_elm = ui::make_element_by_type<ui::diplomacy_request_window>(state, "defaultdialog");
		state.ui_state.request_window = new_elm.get();
		state.ui_state.root->add_child_to_front(std::move(new_elm));
	}
	{
		auto new_elm = ui::make_element_by_type<ui::message_window>(state, "defaultpopup");
		state.ui_state.msg_window = new_elm.get();
		state.ui_state.root->add_child_to_front(std::move(new_elm));
	}
	{
		auto new_elm = ui::make_element_by_type<ui::leader_selection_window>(state, "alice_leader_selection_panel");
		state.ui_state.change_leader_window = new_elm.get();
		state.ui_state.root->add_child_to_front(std::move(new_elm));
	}
	{
		auto new_elm = ui::make_element_by_type<ui::naval_combat_end_popup>(state, "endofnavalcombatpopup");
		new_elm->set_visible(state, false);
		state.ui_state.root->add_child_to_front(std::move(new_elm));
	}
	{
		auto new_elm = ui::make_element_by_type<ui::naval_combat_window>(state, "alice_naval_combat");
		new_elm->set_visible(state, false);
		state.ui_state.root->add_child_to_front(std::move(new_elm));
	}
	{
		auto new_elm = ui::make_element_by_type<ui::land_combat_window>(state, "alice_land_combat");
		new_elm->set_visible(state, false);
		state.ui_state.root->add_child_to_front(std::move(new_elm));
	}
	{
		auto new_elem = make_element_by_type<disband_unit_confirmation>(state, "disband_window");
		// set window to be movable
		new_elem->base_data.data.window.flags |= new_elem->base_data.data.window.is_moveable_mask;
		new_elem->set_visible(state, false);
		state.ui_state.disband_unit_window = new_elem.get();
		state.ui_state.root->add_child_to_front(std::move(new_elem));

	}
	{
		auto new_elm = ui::make_element_by_type<ui::topbar_window>(state, "topbar");
		new_elm->impl_on_update(state);
		state.ui_state.root->add_child_to_front(std::move(new_elm));
	}
	{
		auto legend_win = ui::make_element_by_type<ui::map_legend_gradient>(state, "alice_map_legend_gradient_window");
		state.ui_state.map_gradient_legend = legend_win.get();
		state.ui_state.root->add_child_to_front(std::move(legend_win));
	}
	{
		auto legend_win = ui::make_element_by_type<ui::map_legend_civ_level>(state, "alice_map_legend_civ_level");
		state.ui_state.map_civ_level_legend = legend_win.get();
		state.ui_state.root->add_child_to_front(std::move(legend_win));
	}
	{
		auto legend_win = ui::make_element_by_type<ui::map_legend_col>(state, "alice_map_legend_colonial");
		state.ui_state.map_col_legend = legend_win.get();
		state.ui_state.root->add_child_to_front(std::move(legend_win));
	}
	{
		auto legend_win = ui::make_element_by_type<ui::map_legend_dip>(state, "alice_map_legend_diplomatic");
		state.ui_state.map_dip_legend = legend_win.get();
		state.ui_state.root->add_child_to_front(std::move(legend_win));
	}
	{
		auto legend_win = ui::make_element_by_type<ui::map_legend_rr>(state, "alice_map_legend_infrastructure");
		state.ui_state.map_rr_legend = legend_win.get();
		state.ui_state.root->add_child_to_front(std::move(legend_win));
	}
	{
		auto legend_win = ui::make_element_by_type<ui::map_legend_nav>(state, "alice_map_legend_naval");
		state.ui_state.map_nav_legend = legend_win.get();
		state.ui_state.root->add_child_to_front(std::move(legend_win));
	}
	{
		auto legend_win = ui::make_element_by_type<ui::map_legend_rank>(state, "alice_map_legend_rank");
		state.ui_state.map_rank_legend = legend_win.get();
		state.ui_state.root->add_child_to_front(std::move(legend_win));
	}
	{
		auto legend_win = ui::make_element_by_type<ui::map_legend_rec>(state, "alice_map_legend_rec");
		state.ui_state.map_rec_legend = legend_win.get();
		state.ui_state.root->add_child_to_front(std::move(legend_win));
	}
	{ // And the other on the normal in game UI
		auto new_elm = ui::make_chat_window(state, "ingame_lobby_window");
		new_elm->set_visible(state, !(state.network_mode == sys::network_mode_type::single_player)); // Hidden in singleplayer by default
		state.ui_state.chat_window = new_elm.get(); // Default for singleplayer is the in-game one, lobby one is useless in sp
		state.ui_state.root->add_child_to_front(std::move(new_elm));
	}
	state.ui_state.rgos_root->impl_on_update(state);
	state.ui_state.units_root->impl_on_update(state);
}


void adjust_in_game_windows(sys::state& state) {
	// Clear "center" property so they don't look messed up!
	{
		static const std::string_view elem_names[] = {
			"state_info",
			"production_goods_name",
			"factory_info",
			"new_factory_option",
			"ledger_legend_entry",
			"project_info",
		};
		for(const auto& elem_name : elem_names) {
			auto it = state.ui_state.defs_by_name.find(state.lookup_key(elem_name));
			if(it != state.ui_state.defs_by_name.end()) {
				auto& gfx_def = state.ui_defs.gui[it->second.definition];
				gfx_def.flags &= ~ui::element_data::orientation_mask;
			}
		}
	}
	// Allow user to drag some windows, and only the ones that make sense
	{
		static const std::string_view elem_names[] = {
			"pop_details_win",
			"trade_flow",
			"event_election_window",
			"invest_project_window",
			"ledger",
			"province_view",
			"releaseconfirm",
			"build_factory",
			"defaultdiplomacydialog",
			"gpselectdiplomacydialog",
			"makecbdialog",
			"declarewardialog",
			"setuppeacedialog",
			"setupcrisisbackdowndialog",
			"endofnavalcombatpopup",
			"endoflandcombatpopup",
			"ingame_lobby_window",
			"build_factory"
		};
		for(const auto& elem_name : elem_names) {
			auto it = state.ui_state.defs_by_name.find(state.lookup_key(elem_name));
			if(it != state.ui_state.defs_by_name.end()) {
				auto& gfx_def = state.ui_defs.gui[it->second.definition];
				if(gfx_def.get_element_type() == ui::element_type::window) {
					gfx_def.data.window.flags |= ui::window_data::is_moveable_mask;
				}
			}
		}
	}
	// Nudge, overriden by V2 to be 0 always
	state.ui_defs.gui[state.ui_state.defs_by_name.find(state.lookup_key("decision_entry"))->second.definition].position = ui::xy_pair{ 0, 0 };
	// Find the object id for the main_bg displayed (so we display it before the map).
	// It is the background from topbar windows
	if(state.ui_state.defs_by_name.find(state.lookup_key("bg_main_menus")) != state.ui_state.defs_by_name.end()) {
		state.ui_state.bg_gfx_id = state.ui_defs.gui[state.ui_state.defs_by_name.find(state.lookup_key("bg_main_menus"))->second.definition].data.image.gfx_object;
	} else if(state.ui_state.gfx_by_name.find(state.lookup_key("GFX_bg_main_menus")) != state.ui_state.gfx_by_name.end()) {
		// If some mod has removed the GUI element of background in topbar windows, resort to searching for the GFX by name
		state.ui_state.bg_gfx_id = state.ui_state.gfx_by_name.find(state.lookup_key("GFX_bg_main_menus"))->second;
	}
	// Otherwise the map will be floating in the void

	state.ui_state.nation_picker = ui::make_element_by_type<ui::nation_picker_container>(state, state.ui_state.defs_by_name.find(state.lookup_key("lobby"))->second.definition);
	{
		auto window = ui::make_element_by_type<ui::console_window>(state, "console_wnd");
		state.ui_state.console_window_r = window.get();
		window->set_visible(state, false);
		state.ui_state.nation_picker->add_child_to_front(std::move(window));
	}
	{ // One on the lobby
		auto new_elm = ui::make_chat_window(state, "ingame_lobby_window");
		new_elm->set_visible(state, !(state.network_mode == sys::network_mode_type::single_player)); // Hidden in singleplayer by default
		state.ui_state.r_chat_window = new_elm.get();
		state.ui_state.nation_picker->add_child_to_front(std::move(new_elm));
	}
}
}
