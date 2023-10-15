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
void update_music_track(
		sys::state& state); // linux may not need this function, but I needed to expose a way to switch to the next track upon
												// recieving a notification via a windows event that the current track could be complete

// returns the default click sound -- expect this list of functions to expand as
//    we implement more of the fixed sound effects
audio_instance& get_click_sound(sys::state& state);
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

} // namespace sound
