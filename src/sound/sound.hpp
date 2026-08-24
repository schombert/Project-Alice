#pragma once

#include <stdint.h>

#ifdef _WIN64
#include "sound_win.hpp"
#else
#include "sound_nix.hpp"
#endif

namespace sys {
struct state;
}

namespace sound {

// called on startup and shutdown -- initialize should also load the list of available music files and load sound effects
void initialize_sound_system(sys::state& state); // should be called *after* the window has been created

// these functions are called to change the volume of the currently playing track or effect
void change_effect_volume(sys::state& state, float v);
void change_interface_volume(sys::state& state, float v);
void change_music_volume(sys::state& state, float v);

// these start playing an effect or track at the specified volume
void play_effect(sys::state& state, audio_instance& s, float volume);
void play_interface_sound(sys::state& state, audio_instance& s, float volume);

// controls autoplaying music (start music should start with the main theme)
void stop_music(sys::state& state);
void start_music(sys::state& state, float v);
void pause_all(sys::state& state);
void resume_all(sys::state& state);
// linux may not need this function, but I needed to expose a way to switch to the next track upon
// recieving a notification via a windows event that the current track could be complete
void update_music_track(sys::state& state);
void play_new_track(sys::state& state);
void play_next_track(sys::state& state);
void play_previous_track(sys::state& state);
native_string get_current_track_name(sys::state& state);

// returns the default click sound -- expect this list of functions to expand as
//    we implement more of the fixed sound effects
audio_instance& get_click_sound(sys::state& state);
audio_instance& get_click_left_sound(sys::state& state);
audio_instance& get_click_right_sound(sys::state& state);
audio_instance& get_tab_budget_sound(sys::state& state);
audio_instance& get_hover_sound(sys::state& state);
audio_instance& get_checkbox_sound(sys::state& state);
audio_instance& get_enact_sound(sys::state& state);
audio_instance& get_subtab_sound(sys::state& state);
audio_instance& get_delete_sound(sys::state& state);
audio_instance& get_autochoose_sound(sys::state& state);
audio_instance& get_tab_politics_sound(sys::state& state);
audio_instance& get_tab_diplomacy_sound(sys::state& state);
audio_instance& get_tab_military_sound(sys::state& state);
audio_instance& get_tab_population_sound(sys::state& state);
audio_instance& get_tab_production_sound(sys::state& state);
audio_instance& get_tab_technology_sound(sys::state& state);
audio_instance& get_army_select_sound(sys::state& state);
audio_instance& get_army_move_sound(sys::state& state);
audio_instance& get_navy_select_sound(sys::state& state);
audio_instance& get_navy_move_sound(sys::state& state);
audio_instance& get_error_sound(sys::state& state);
audio_instance& get_peace_sound(sys::state& state);
audio_instance& get_army_built_sound(sys::state& state);
audio_instance& get_navy_built_sound(sys::state& state);
audio_instance& get_declaration_of_war_sound(sys::state& state);
audio_instance& get_technology_finished_sound(sys::state& state);
audio_instance& get_factory_built_sound(sys::state& state);
audio_instance& get_election_sound(sys::state& state);
audio_instance& get_revolt_sound(sys::state& state);
audio_instance& get_fort_built_sound(sys::state& state);
audio_instance& get_railroad_built_sound(sys::state& state);
audio_instance& get_naval_base_built_sound(sys::state& state);
audio_instance& get_minor_event_sound(sys::state& state);
audio_instance& get_major_event_sound(sys::state& state);
audio_instance& get_decline_sound(sys::state& state);
audio_instance& get_accept_sound(sys::state& state);
audio_instance& get_diplomatic_request_sound(sys::state& state);
audio_instance& get_chat_message_sound(sys::state& state);
//alice
audio_instance& get_console_open_sound(sys::state& state);
audio_instance& get_console_close_sound(sys::state& state);
audio_instance& get_event_sound(sys::state& state);
audio_instance& get_decision_sound(sys::state& state);
audio_instance& get_pause_sound(sys::state& state);
audio_instance& get_unpause_sound(sys::state& state);

audio_instance& get_enemycapitulated_sound(sys::state& state);
audio_instance& get_wecapitulated_sound(sys::state& state);

audio_instance& get_random_land_battle_sound(sys::state& state);
audio_instance& get_random_naval_battle_sound(sys::state& state);
audio_instance& get_random_province_select_sound(sys::state& state);

} // namespace sound
