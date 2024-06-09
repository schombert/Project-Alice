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
	~audio_instance() { }
	void set_file(native_string_view name) {
		filename = native_string(name);
	}
};

class sound_impl {
public:
	std::optional<ma_sound> effect_sound;
	std::optional<ma_sound> interface_sound;
	std::optional<ma_sound> music;

	ma_engine engine;
	bool global_pause = false;

	audio_instance click_sound;
	audio_instance click_left_sound;
	audio_instance click_right_sound;
	audio_instance tab_budget_sound;
	audio_instance hover_sound;
	audio_instance checkbox_sound;
	audio_instance enact_sound;
	audio_instance subtab_sound;
	audio_instance delete_sound;
	audio_instance autochoose_sound;
	audio_instance tab_politics_sound;
	audio_instance tab_diplomacy_sound;
	audio_instance tab_military_sound;
	audio_instance tab_population_sound;
	audio_instance tab_production_sound;
	audio_instance tab_technology_sound;
	audio_instance technology_finished_sound;
	audio_instance army_move_sound;
	audio_instance army_select_sound;
	audio_instance navy_move_sound;
	audio_instance navy_select_sound;
	audio_instance declaration_of_war_sound;
	audio_instance chat_message_sound;
	audio_instance error_sound;
	audio_instance peace_sound;
	audio_instance army_built_sound;
	audio_instance navy_built_sound;
	audio_instance factory_built_sound;
	audio_instance election_sound;
	audio_instance revolt_sound;
	audio_instance fort_built_sound;
	audio_instance railroad_built_sound;
	audio_instance naval_base_built_sound;
	audio_instance minor_event_sound;
	audio_instance major_event_sound;
	audio_instance decline_sound;
	audio_instance accept_sound;
	audio_instance diplomatic_request_sound;
	audio_instance console_open_sound;
	audio_instance console_close_sound;
	audio_instance land_battle_sounds[6];
	audio_instance naval_battle_sounds[6];
	audio_instance province_select_sounds[4];
	audio_instance event_sound;
	audio_instance decision_sound;
	audio_instance pause_sound;
	audio_instance unpause_sound;

	std::vector<audio_instance> music_list;
	int32_t last_music = -1;
	int32_t first_music = -1;
	int32_t current_music = -1;

	sound_impl();
	~sound_impl();
	void set_volume(std::optional<ma_sound>& sound, float volume);
	void override_sound(std::optional<ma_sound>& sound, audio_instance& s, float volume);
	void play_music(int32_t track, float volume);
	void play_new_track(sys::state& ws);
	void play_next_track(sys::state& ws);
	void play_previous_track(sys::state& ws);
	bool music_finished();
};

} // namespace sound
