#pragma once

#include<stdint.h>

namespace sys {
struct state;
}

namespace sound {

class sound_impl; // use this to store implementation dependent data
class audio_instance; // implementation dependent structure for storing a single music/audio file

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
void update_music_track(sys::state& state); // linux may not need this function, but I needed to expose a way to switch to the next track upon recieving a notification via a windows event that the current track could be complete

// returns the default click sound -- expect this list of functions to expand as
//    we implement more of the fixed sound effects
audio_instance& get_click_sound(sys::state& state);

}
