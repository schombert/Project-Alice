#include "sound.hpp"
#include "system_state.hpp"

namespace sound {

class sound_impl { };
class audio_instance { };

void initialize_sound_system(sys::state& state) { }
void change_effect_volume(sys::state& state, float v) { }
void change_interface_volume(sys::state& state, float v) { }
void change_music_volume(sys::state& state, float v) { }

void play_effect(sys::state& state, audio_instance& s, float volume) { }
void play_interface_sound(sys::state& state, audio_instance& s, float volume) { }

void stop_music(sys::state& state) { }
void start_music(sys::state& state, float v) { }
void update_music_track(sys::state& state) { }
audio_instance& get_click_sound(sys::state& state) {
	static audio_instance temp; // DO NOT use static variables this way; this is just to make the stub work
	return temp;
}

}
