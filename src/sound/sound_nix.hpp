#pragma once

#include "system_state.hpp"

#include "miniaudio.h"

namespace sound {

class audio_instance {
public:
	native_string filename;

	audio_instance() = default;

	audio_instance& operator=(audio_instance const& o) {
		filename = o.filename;
		return *this;
	}

	audio_instance(simple_fs::unopened_file const& file) {
		filename = simple_fs::get_full_name(file);
	}

	~audio_instance() { }
};

class sound_impl {
public:
	std::optional<ma_sound> effect_sound;
	std::optional<ma_sound> interface_sound;
	std::optional<ma_sound> music;

	ma_engine engine;
	audio_instance click_sound;
	std::vector<audio_instance> music_list;
	int32_t last_music = -1;
	int32_t first_music = -1;
	int32_t current_music = -1;

	sound_impl();

	~sound_impl();

	void set_volume(std::optional<ma_sound>& sound, float volume);

	void override_sound(std::optional<ma_sound>& sound, audio_instance& s, float volume);

	void play_music(int32_t track, float volume);

	void play_new_track(sys::state& s, float v);

	bool music_finished();
};

} // namespace sound
