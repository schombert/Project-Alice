#include "sound.hpp"
#include "system_state.hpp"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

namespace sound {

class audio_instance {
public:
	native_string filename;
	ma_sound sound;

	audio_instance() = default;
	audio_instance& operator=(audio_instance const& o) {
		filename = o.filename;
		sound = o.sound;
		return *this;
	}

	audio_instance(simple_fs::unopened_file const& file) {
		filename = simple_fs::get_full_name(file);
	}

	~audio_instance() {

	}
};

class sound_impl {
public:
	ma_engine engine;
	audio_instance click_sound;
	std::vector<audio_instance> music_list;
	int32_t last_music = -1;
	int32_t first_music = -1;

	sound_impl() {
		if(ma_engine_init(NULL, &engine) != MA_SUCCESS) {
			std::abort();
		}
	}

	~sound_impl() {
		ma_engine_uninit(&engine);
	}

	void play_music(int32_t track, float volume) {
		ma_engine_play_sound(&engine, music_list[track].filename.c_str(), NULL);
	}

	void play_new_track(sys::state& s, float v) {
		if(music_list.size() > 0) {
			int32_t result = int32_t(rand() % music_list.size()); // well aware that using rand is terrible, thanks
			while(result == last_music)
				result = int32_t(rand() % music_list.size());
			play_music(result, v);
		}
	}
};

void initialize_sound_system(sys::state& state) {
	state.sound_ptr = std::make_unique<sound_impl>();

	auto root = get_root(state.common_fs);

	const auto music_directory = open_directory(root, NATIVE("music"));
	for(const auto& mp3_file : list_files(music_directory , NATIVE(".mp3"))) {
		state.sound_ptr->music_list.emplace_back(mp3_file);

		auto file_name = get_full_name(mp3_file);
		if(parsers::native_has_fixed_suffix_ci(file_name.c_str(), file_name.c_str() + file_name.length(), NATIVE("thecoronation_titletheme.mp3")))
			state.sound_ptr->first_music = int32_t(state.sound_ptr->music_list.size()) - 1;
	}

	const auto sound_directory = open_directory(root, NATIVE("sound"));
	auto click_peek = peek_file(sound_directory, NATIVE("GI_ValidClick.wav"));
	if(!click_peek) {
		printf("can't open click sound\n");
		std::abort();
	}
	state.sound_ptr->click_sound = audio_instance(*click_peek);
}
void change_effect_volume(sys::state& state, float v) { }
void change_interface_volume(sys::state& state, float v) { }
void change_music_volume(sys::state& state, float v) { }

void play_effect(sys::state& state, audio_instance& s, float volume) {
	ma_engine_play_sound(&state.sound_ptr->engine, s.filename.c_str(), NULL);
}
void play_interface_sound(sys::state& state, audio_instance& s, float volume) {
	ma_engine_play_sound(&state.sound_ptr->engine, s.filename.c_str(), NULL);
}

void stop_music(sys::state& state) { }
void start_music(sys::state& state, float v) {
	state.sound_ptr->play_new_track(state, v);
}
void update_music_track(sys::state& state) { }
audio_instance& get_click_sound(sys::state& state) {
	return state.sound_ptr->click_sound;
}

}
