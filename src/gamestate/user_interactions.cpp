#include "system_state.hpp"
#include "gui_element_base.hpp"
#include "gui_event.hpp"
#include "gui_combat.hpp"
#include "gui_diplomacy_request_templates.hpp"
#include "gui_message_settings_window.hpp"
#include "gui_message_window.hpp"
#include "gui_templates.hpp"
#include "gui_error_window.hpp"
#include "commands.hpp"

namespace sys {

void on_mouse_wheel(sys::state& state, int32_t x, int32_t y, key_modifiers mod, float amount) { // an amount of 1.0 is one "click" of the wheel
	ui::element_base* root_elm = state.current_scene.get_root(state);
	auto probe_result = root_elm->impl_probe_mouse(state,
		int32_t(state.mouse_x_position / state.user_settings.ui_scale),
		int32_t(state.mouse_y_position / state.user_settings.ui_scale),
		ui::mouse_probe_type::scroll);

	state.ui_state.scroll_target = probe_result.under_mouse;

	auto belongs_on_map = [&](ui::element_base* b) {
		while(b != nullptr) {
			if(b == state.ui_state.units_root.get())
				return true;
			if(b == state.ui_state.unit_details_box.get())
				return true;
			b = b->parent;
		}
		return false;
	};

	if(state.ui_state.scroll_target != nullptr) {
		state.ui_state.scroll_target->impl_on_scroll(state, probe_result.relative_location.x, probe_result.relative_location.y, amount, mod);
	} else if(state.ui_state.under_mouse == nullptr || belongs_on_map(state.ui_state.under_mouse)) {
		state.map_state.on_mouse_wheel(x, y, state.x_size, state.y_size, mod, amount);

		if(state.ui_state.mouse_sensitive_target) {
			state.ui_state.mouse_sensitive_target->set_visible(state, false);
			state.ui_state.mouse_sensitive_target = nullptr;
		}
	}
}

void process_errorpopup_boxes(sys::state& state) {
	auto* lr = state.error_windows.front();
	while(lr) {
		ui::popup_error_window(state, lr->header_text, lr->description_text);
		state.error_windows.pop();
		lr = state.error_windows.front();
	}
}

void process_dialog_boxes(sys::state& state) {
	// National events
	auto* c1 = state.new_n_event.front();
	while(c1) {
		auto auto_choice = state.world.national_event_get_auto_choice(c1->e);
		if(auto_choice == 0) {
			ui::new_event_window(state, *c1);
			if(state.world.national_event_get_is_major(c1->e)) {
				sound::play_effect(state, sound::get_major_event_sound(state), state.user_settings.effects_volume * state.user_settings.master_volume);
			} else {
				sound::play_effect(state, sound::get_major_event_sound(state), state.user_settings.effects_volume * state.user_settings.master_volume);
			}
		} else {
			command::make_event_choice(state, *c1, uint8_t(auto_choice - 1));
		}
		state.new_n_event.pop();
		c1 = state.new_n_event.front();
	}
	// Free national events
	auto* c2 = state.new_f_n_event.front();
	while(c2) {
		auto auto_choice = state.world.free_national_event_get_auto_choice(c2->e);
		if(auto_choice == 0) {
			ui::new_event_window(state, *c2);
			if(state.world.free_national_event_get_is_major(c2->e)) {
				sound::play_effect(state, sound::get_major_event_sound(state), state.user_settings.effects_volume * state.user_settings.master_volume);
			} else {
				sound::play_effect(state, sound::get_major_event_sound(state), state.user_settings.effects_volume * state.user_settings.master_volume);
			}
		} else {
			command::make_event_choice(state, *c2, uint8_t(auto_choice - 1));
		}
		state.new_f_n_event.pop();
		c2 = state.new_f_n_event.front();
	}
	// Provincial events
	auto* c3 = state.new_p_event.front();
	while(c3) {
		auto auto_choice = state.world.provincial_event_get_auto_choice(c3->e);
		if(auto_choice == 0) {
			ui::new_event_window(state, *c3);
			sound::play_effect(state, sound::get_minor_event_sound(state), state.user_settings.effects_volume * state.user_settings.master_volume);
		} else {
			command::make_event_choice(state, *c3, uint8_t(auto_choice - 1));
		}
		state.new_p_event.pop();
		c3 = state.new_p_event.front();
	}
	// Free provincial events
	auto* c4 = state.new_f_p_event.front();
	while(c4) {
		auto auto_choice = state.world.free_provincial_event_get_auto_choice(c4->e);
		if(auto_choice == 0) {
			ui::new_event_window(state, *c4);
			sound::play_effect(state, sound::get_minor_event_sound(state), state.user_settings.effects_volume * state.user_settings.master_volume);
		} else {
			command::make_event_choice(state, *c4, uint8_t(auto_choice - 1));
		}
		state.new_f_p_event.pop();
		c4 = state.new_f_p_event.front();
	}
	// land battle reports
	{
		auto* lr = state.land_battle_reports.front();
		while(lr) {
			if(state.local_player_nation) {
				if(lr->player_on_winning_side == true && (!lr->attacking_nation || !lr->defending_nation)) {
					if(state.user_settings.notify_rebels_defeat) {
						ui::land_combat_end_popup::make_new_report(state, *lr);
					} else {
						//do not pester user with defeat of rebels
					}
				} else {
					ui::land_combat_end_popup::make_new_report(state, *lr);
				}
			}
			state.land_battle_reports.pop();
			lr = state.land_battle_reports.front();
		}
	}
	// naval battle reports
	{
		auto* lr = state.naval_battle_reports.front();
		while(lr) {
			ui::naval_combat_end_popup::make_new_report(state, *lr);
			state.naval_battle_reports.pop();
			lr = state.naval_battle_reports.front();
		}
	}
	// Diplomatic messages
	auto* c5 = state.new_requests.front();
	bool had_diplo_msg = false;
	while(c5) {
		if(state.user_settings.diplomatic_message_popup) {
			static_cast<ui::diplomacy_request_window*>(state.ui_state.request_window)->messages.push_back(*c5);
		} else {
			static_cast<ui::diplomatic_message_topbar_listbox*>(state.ui_state.request_topbar_listbox)->messages.push_back(*c5);
		}
		had_diplo_msg = true;
		state.new_requests.pop();
		c5 = state.new_requests.front();
	}
	if(had_diplo_msg) {
		sound::play_effect(state, sound::get_diplomatic_request_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);
	}

	// Log messages
	auto* c6 = state.new_messages.front();
	while(c6) {
		auto base_type = c6->type;
		auto setting_types = sys::message_setting_map[int32_t(base_type)];
		uint8_t settings_bits = 0;
		if(setting_types.source != sys::message_setting_type::count) {
			if(c6->source == state.local_player_nation) {
				settings_bits |= state.user_settings.self_message_settings[int32_t(setting_types.source)];
			} else if(notification::nation_is_interesting(state, c6->source)) {
				settings_bits |= state.user_settings.interesting_message_settings[int32_t(setting_types.source)];
			} else {
				settings_bits |= state.user_settings.other_message_settings[int32_t(setting_types.source)];
			}
		}
		if(setting_types.target != sys::message_setting_type::count) {
			if(c6->target == state.local_player_nation) {
				settings_bits |= state.user_settings.self_message_settings[int32_t(setting_types.target)];
			} else if(notification::nation_is_interesting(state, c6->target)) {
				settings_bits |= state.user_settings.interesting_message_settings[int32_t(setting_types.target)];
			} else {
				settings_bits |= state.user_settings.other_message_settings[int32_t(setting_types.target)];
			}
		}
		if(setting_types.third != sys::message_setting_type::count) {
			if(c6->third == state.local_player_nation) {
				settings_bits |= state.user_settings.self_message_settings[int32_t(setting_types.third)];
			} else if(notification::nation_is_interesting(state, c6->third)) {
				settings_bits |= state.user_settings.interesting_message_settings[int32_t(setting_types.third)];
			} else {
				settings_bits |= state.user_settings.other_message_settings[int32_t(setting_types.third)];
			}
		}

		if((settings_bits & message_response::log) && state.ui_state.msg_log_window) {
			static_cast<ui::message_log_window*>(state.ui_state.msg_log_window)->messages.push_back(*c6);
		}
		if(settings_bits & message_response::popup) {
			if(c6->source == state.local_player_nation && (base_type == message_base_type::major_event || base_type == message_base_type::national_event || base_type == message_base_type::province_event)) {
				// do nothing -- covered by event window logic
			} else {
				if(state.ui_state.msg_window) {
					static_cast<ui::message_window*>(state.ui_state.msg_window)->messages.push_back(*c6);
				}
				if((settings_bits & message_response::pause) != 0 && state.network_mode == sys::network_mode_type::single_player) {
					state.ui_pause.store(true, std::memory_order_release);
				}
			}
		}


		// Sound effects(tm)
		if(settings_bits != 0 && state.local_player_nation && (c6->source == state.local_player_nation || c6->target == state.local_player_nation)) {
			switch(base_type) {
			case message_base_type::war:
				sound::play_effect(state, sound::get_declaration_of_war_sound(state), state.user_settings.effects_volume * state.user_settings.master_volume);
				break;
			case message_base_type::peace_accepted:
				sound::play_effect(state, sound::get_peace_sound(state), state.user_settings.effects_volume * state.user_settings.master_volume);
				break;
			case message_base_type::tech:
				sound::play_effect(state, sound::get_technology_finished_sound(state), state.user_settings.effects_volume * state.user_settings.master_volume);
				break;
			case message_base_type::factory_complete:
				sound::play_effect(state, sound::get_factory_built_sound(state), state.user_settings.effects_volume * state.user_settings.master_volume);
				break;
			case message_base_type::fort_complete:
				sound::play_effect(state, sound::get_fort_built_sound(state), state.user_settings.effects_volume * state.user_settings.master_volume);
				break;
			case message_base_type::rr_complete:
				sound::play_effect(state, sound::get_railroad_built_sound(state), state.user_settings.effects_volume * state.user_settings.master_volume);
				break;
			case message_base_type::naval_base_complete:
				sound::play_effect(state, sound::get_naval_base_built_sound(state), state.user_settings.effects_volume * state.user_settings.master_volume);
				break;
			case message_base_type::electionstart:
			case message_base_type::electiondone:
				sound::play_effect(state, sound::get_election_sound(state), state.user_settings.effects_volume * state.user_settings.master_volume);
				break;
			case message_base_type::revolt:
				sound::play_effect(state, sound::get_revolt_sound(state), state.user_settings.effects_volume * state.user_settings.master_volume);
				break;
			case message_base_type::army_built:
				sound::play_effect(state, sound::get_army_built_sound(state), state.user_settings.effects_volume * state.user_settings.master_volume);
				break;
			case message_base_type::navy_built:
				sound::play_effect(state, sound::get_navy_built_sound(state), state.user_settings.effects_volume * state.user_settings.master_volume);
				break;
			case message_base_type::alliance_declined:
			case message_base_type::ally_called_declined:
			case message_base_type::crisis_join_offer_declined:
			case message_base_type::crisis_resolution_declined:
			case message_base_type::mil_access_declined:
			case message_base_type::peace_rejected:
				sound::play_effect(state, sound::get_decline_sound(state), state.user_settings.effects_volume * state.user_settings.master_volume);
				break;
			case message_base_type::alliance_starts:
			case message_base_type::ally_called_accepted:
			case message_base_type::crisis_join_offer_accepted:
			case message_base_type::crisis_resolution_accepted:
			case message_base_type::mil_access_start:
				sound::play_effect(state, sound::get_accept_sound(state), state.user_settings.effects_volume * state.user_settings.master_volume);
				break;
			case message_base_type::chat_message:
				sound::play_interface_sound(state, sound::get_chat_message_sound(state), state.user_settings.effects_volume * state.user_settings.master_volume);
				break;
			case message_base_type::province_event:
			case message_base_type::national_event:
			case message_base_type::major_event:
				//Sound effect is played on above logic (free/non-free loop events above)
				break;
			default:
				break;
			}
		}

		state.new_messages.pop();
		c6 = state.new_messages.front();
	}
	// Naval Combat Reports
	auto* c7 = state.naval_battle_reports.front();
	while(c7) {
		if(state.ui_state.endof_navalcombat_windows.size() == 0) {
			state.ui_state.endof_navalcombat_windows.push_back(ui::make_element_by_type<ui::naval_combat_end_popup>(state,
				state.ui_state.defs_by_name.find(state.lookup_key("endofnavalcombatpopup"))->second.definition));
		}
		//static_cast<ui::naval_combat_window*>(ui_state.navalcombat_windows.back().get())->messages.push_back(*c7);
		static_cast<ui::naval_combat_end_popup*>(state.ui_state.endof_navalcombat_windows.back().get())->report = *c7;
		state.ui_state.root->add_child_to_front(std::move(state.ui_state.endof_navalcombat_windows.back()));
		state.ui_state.endof_navalcombat_windows.pop_back();
		state.naval_battle_reports.pop();
		c7 = state.naval_battle_reports.front();
	}
	if(!static_cast<ui::diplomacy_request_window*>(state.ui_state.request_window)->messages.empty()) {
		state.ui_state.request_window->set_visible(state, true);
		state.ui_state.root->move_child_to_front(state.ui_state.request_window);
	}
	if(!static_cast<ui::message_window*>(state.ui_state.msg_window)->messages.empty()) {
		state.ui_state.msg_window->set_visible(state, true);
		state.ui_state.root->move_child_to_front(state.ui_state.msg_window);
	}
}

void open_diplomacy_window(sys::state& state, dcon::nation_id target) {
	if(state.ui_state.diplomacy_subwindow != nullptr) {
		if(state.ui_state.topbar_subwindow != nullptr) {
			state.ui_state.topbar_subwindow->set_visible(state, false);
		}
		state.ui_state.topbar_subwindow = state.ui_state.diplomacy_subwindow;
		state.ui_state.diplomacy_subwindow->set_visible(state, true);
		state.ui_state.root->move_child_to_front(state.ui_state.diplomacy_subwindow);
		send(state, state.ui_state.diplomacy_subwindow, ui::element_selection_wrapper<dcon::nation_id>{ target });
	}
}

}
