#include "sound.hpp"
#include "system_state.hpp"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

namespace sound {

class audio_instance {
  public:
	native_string filename;

	audio_instance() = default;

	audio_instance &operator=(audio_instance const &o) {
		filename = o.filename;
		return *this;
	}

	audio_instance(simple_fs::unopened_file const &file) {
		filename = simple_fs::get_full_name(file);
	}

	~audio_instance() {
	}
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

	sound_impl() {
		if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
			std::abort();
		}
	}

	~sound_impl() {
		ma_engine_uninit(&engine);
	}

	void set_volume(std::optional<ma_sound> &sound, float volume) {
		if (sound.has_value()) {
			ma_sound_set_volume(&*sound, volume);
		}
	}

	void override_sound(std::optional<ma_sound> &sound, audio_instance &s, float volume) {
		if (sound.has_value()) {
			ma_sound_uninit(&*sound);
		}

		sound.reset();
		sound.emplace();
		ma_result result = ma_sound_init_from_file(&engine, s.filename.c_str(), 0, NULL, NULL, &*sound);
		if (result == MA_SUCCESS) {
			set_volume(sound, volume);
			ma_sound_start(&*sound);
		}
	}

	void play_music(int32_t track, float volume) {
		current_music = track;

		audio_instance audio{};
		audio.filename = music_list[track].filename.c_str();
		override_sound(music, audio, volume);
	}

	void play_new_track(sys::state &s, float v) {
		if (music_list.size() > 0) {
			int32_t result = int32_t(rand() % music_list.size()); // well aware that using rand is terrible, thanks
			while (result == last_music)
				result = int32_t(rand() % music_list.size());
			play_music(result, v);
		}
	}

	bool music_finished() {
		if (music.has_value())
			return music->atEnd;
		return true;
	}
};

void initialize_sound_system(sys::state &state) {
	state.sound_ptr = std::make_unique<sound_impl>();

	auto root = get_root(state.common_fs);

	const auto music_directory = open_directory(root, NATIVE("music"));
	for (const auto &mp3_file : list_files(music_directory, NATIVE(".mp3"))) {
		state.sound_ptr->music_list.emplace_back(mp3_file);

		auto file_name = get_full_name(mp3_file);
		if (parsers::native_has_fixed_suffix_ci(file_name.c_str(), file_name.c_str() + file_name.length(), NATIVE("thecoronation_titletheme.mp3")))
			state.sound_ptr->first_music = int32_t(state.sound_ptr->music_list.size()) - 1;
	}

	const auto sound_directory = open_directory(root, NATIVE("sound"));
	auto click_peek = peek_file(sound_directory, NATIVE("GI_ValidClick.wav"));
	if (!click_peek) {
		printf("can't open click sound\n");
		std::abort();
	}
	state.sound_ptr->click_sound = audio_instance(*click_peek);
}
void change_effect_volume(sys::state &state, float v) {
	state.sound_ptr->set_volume(state.sound_ptr->effect_sound, v);
}
void change_interface_volume(sys::state &state, float v) {
	state.sound_ptr->set_volume(state.sound_ptr->interface_sound, v);
}
void change_music_volume(sys::state &state, float v) {
	state.sound_ptr->set_volume(state.sound_ptr->music, v);
}

void play_effect(sys::state &state, audio_instance &s, float volume) {
	state.sound_ptr->override_sound(state.sound_ptr->effect_sound, s, volume);
}
void play_interface_sound(sys::state &state, audio_instance &s, float volume) {
	state.sound_ptr->override_sound(state.sound_ptr->interface_sound, s, volume);
}

void stop_music(sys::state &state) {
	if (state.sound_ptr->music.has_value()) {
		ma_sound_stop(&*state.sound_ptr->music);
	}
}
void start_music(sys::state &state, float v) {
	if (state.sound_ptr->music.has_value()) {
		ma_sound_start(&*state.sound_ptr->music);
	}
}
void update_music_track(sys::state &state) {
	if (state.sound_ptr->music_finished()) {
		state.sound_ptr->play_new_track(state, state.user_settings.master_volume * state.user_settings.music_volume);
	}
}
audio_instance &get_click_sound(sys::state &state) {
	return state.sound_ptr->click_sound;
}

} // namespace sound
