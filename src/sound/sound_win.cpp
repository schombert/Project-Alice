#include "sound.hpp"
#include "system_state.hpp"
#include "Dshow.h"

#define WM_GRAPHNOTIFY (WM_APP + 1)

#pragma comment(lib, "Strmiids.lib")

namespace sound {

constexpr int32_t volume_function(float v) {
	return std::clamp(int32_t((v + -1.0f) * 4'500.0f), -10'000, 0);
}

audio_instance::~audio_instance() {
	if(audio_interface) {
		((IBasicAudio*)audio_interface)->Release();
		audio_interface = nullptr;
	}
	if(control_interface) {
		((IMediaControl*)control_interface)->Release();
		control_interface = nullptr;
	}
	if(seek_interface) {
		((IMediaSeeking*)seek_interface)->Release();
		seek_interface = nullptr;
	}
	if(event_interface) {
		((IMediaEventEx*)event_interface)->SetNotifyWindow(NULL, 0, NULL);
		((IMediaEventEx*)event_interface)->Release();
		event_interface = nullptr;
	}
	if(graph_interface) {
		((IGraphBuilder*)graph_interface)->Release();
		graph_interface = nullptr;
	}
}

void audio_instance::play(float volume, bool as_music, void* window_handle) {
	if(volume * volume_multiplier <= 0.0f || filename.size() == 0)
		return;

	if(!graph_interface) {
		IGraphBuilder* pGraph = nullptr;

		HRESULT hr = CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&pGraph);
		if(FAILED(hr)) {
			window::emit_error_message("failed to create graph builder", false);
			return;
		}

		hr = pGraph->RenderFile((wchar_t const*)(filename.c_str()), nullptr);
		if(FAILED(hr)) {
			window::emit_error_message("unable to play audio file", false);
			volume_multiplier = 0.0f;
			return;
		}
		IMediaControl* pControl = nullptr;
		hr = pGraph->QueryInterface(IID_IMediaControl, (void**)&pControl);
		if(FAILED(hr)) {
			window::emit_error_message("failed to get control interface", false);
			return;
		}
		if(as_music) {
			IMediaEventEx* pEvent = nullptr;
			hr = pGraph->QueryInterface(IID_IMediaEventEx, (void**)&pEvent);
			if(FAILED(hr)) {
				window::emit_error_message("failed to get event interface", false);
				return;
			}
			auto const res_b = pEvent->SetNotifyWindow((OAHWND)window_handle, WM_GRAPHNOTIFY, NULL);
			if(FAILED(res_b)) {
				window::emit_error_message("failed to set notification window", false);
				return;
			}
			event_interface = pEvent;
		}
		IBasicAudio* pAudio = nullptr;
		hr = pGraph->QueryInterface(IID_IBasicAudio, (void**)&pAudio);
		if(FAILED(hr)) {
			window::emit_error_message("failed to get audio interface", false);
			return;
		}
		IMediaSeeking* pSeek = nullptr;
		hr = pGraph->QueryInterface(IID_IMediaSeeking, (void**)&pSeek);
		if(FAILED(hr)) {
			window::emit_error_message("failed to get seeking interface", false);
			return;
		}
		graph_interface = pGraph;
		control_interface = pControl;
		audio_interface = pAudio;
		seek_interface = pSeek;
		hr = ((IBasicAudio*)pAudio)->put_Volume(volume_function(volume * volume_multiplier));
		if(FAILED(hr)) {
			window::emit_error_message("failed to put_Volume", false);
		}
		LONGLONG new_position = 0;
		hr = ((IMediaSeeking*)pSeek)->SetPositions(&new_position, AM_SEEKING_AbsolutePositioning, nullptr, AM_SEEKING_NoPositioning);
		if(FAILED(hr)) {
			window::emit_error_message("failed to SetPositions", false);
		}
		hr = ((IMediaControl*)pControl)->Run();
		if(FAILED(hr)) {
			window::emit_error_message("failed to Run", false);
		}
	} else {
		HRESULT hr;
		if(audio_interface) {
			hr = audio_interface->put_Volume(volume_function(volume * volume_multiplier));
			if(FAILED(hr)) {
				window::emit_error_message("failed to put_Volume", false);
			}
		}
		if(seek_interface) {
			LONGLONG new_position = 0;
			hr = seek_interface->SetPositions(&new_position, AM_SEEKING_AbsolutePositioning, nullptr, AM_SEEKING_NoPositioning);
			if(FAILED(hr)) {
				window::emit_error_message("failed to SetPositions", false);
			}
		}
		if(control_interface) {
			hr = control_interface->Run();
			if(FAILED(hr)) {
				window::emit_error_message("failed to Run", false);
			}
		}
	}
}

void audio_instance::pause() const {
	if(control_interface)
		control_interface->Pause();
}
void audio_instance::resume() const {
	if(control_interface)
		control_interface->Run();
}
void audio_instance::stop() const {
	if(control_interface)
		control_interface->Pause();
}

bool audio_instance::is_playing() const {
	if(seek_interface && volume_multiplier > 0.0f) {
		LONGLONG end_position = 0;
		LONGLONG current_position = 0;
		auto const result = seek_interface->GetPositions(&current_position, &end_position);

		return !(FAILED(result) || current_position >= end_position);
	} else {
		return false;
	}
}

void audio_instance::change_volume(float new_volume) const {
	if(new_volume * volume_multiplier > 0.0f) {
		if(audio_interface)
			audio_interface->put_Volume(volume_function(new_volume * volume_multiplier));
	} else {
		if(control_interface)
			control_interface->Pause();
	}
}

void sound_impl::play_new_track(sys::state& ws) {
	if(music_list.size() > 0) {
		int32_t result = int32_t(rand() % music_list.size()); // well aware that using rand is terrible, thanks
		for(uint32_t i = 0; i < 16 && result == last_music; i++) {
			result = int32_t(rand() % music_list.size());
		}
		play_music(result, ws.user_settings.master_volume * ws.user_settings.music_volume);
	}
}
void sound_impl::play_next_track(sys::state& ws) {
	if(music_list.size() > 0) {
		int32_t result = int32_t((last_music + 1) % music_list.size());
		play_music(result, ws.user_settings.master_volume * ws.user_settings.music_volume);
	}
}
void sound_impl::play_previous_track(sys::state& ws) {
	if(music_list.size() > 0) {
		int32_t result = int32_t((last_music - 1) % music_list.size());
		play_music(result, ws.user_settings.master_volume * ws.user_settings.music_volume);
	}
}

bool sound_impl::music_finished() const {
	auto const lm = last_music;
	if(lm == -1)
		return false;

	long evCode;
	LONG_PTR param1, param2;
	auto const event_interface = music_list[lm].event_interface;
	if(event_interface) {
		while(SUCCEEDED(event_interface->GetEvent(&evCode, &param1, &param2, 0))) {
			event_interface->FreeEventParams(evCode, param1, param2);
			switch(evCode) {
			case EC_COMPLETE:	 // Fall through.
			case EC_USERABORT: // Fall through.
				return true;
			default:;
				// nothing
			}
		}
		return false;
	} else {
		return false;
	}
}

void sound_impl::play_effect(audio_instance& s, float volume) {
	if(global_pause)
		return;

	if(!current_effect || current_effect->is_playing() == false) {
		current_effect = &s;
		s.play(volume, false, window_handle);
	}
}

void sound_impl::play_interface_sound(audio_instance& s, float volume) {
	if(global_pause)
		return;

	if(current_interface_sound)
		current_interface_sound->stop();
	current_interface_sound = &s;
	s.play(volume, false, window_handle);
}

void sound_impl::play_music(int32_t track, float volume) {
	if(global_pause)
		return;

	auto const lm = last_music;
	if(lm != -1)
		music_list[lm].stop();
	last_music = track;
	if(track != -1)
		music_list[track].play(volume, true, window_handle);
}

void sound_impl::change_effect_volume(float v) const {
	if(current_effect)
		current_effect->change_volume(v);
}

void sound_impl::change_interface_volume(float v) const {
	if(current_interface_sound)
		current_interface_sound->change_volume(v);
}

void sound_impl::change_music_volume(float v) const {
	auto const lm = last_music;
	if(lm != -1) {
		music_list[lm].change_volume(v);
	}
}

void sound_impl::pause_effect() const {
	if(current_effect)
		current_effect->pause();
}
void sound_impl::pause_interface_sound() const {
	if(current_interface_sound)
		current_interface_sound->pause();
}
void sound_impl::pause_music() const {
	auto const lm = last_music;
	if(lm != -1) {
		music_list[lm].pause();
	}
}

void sound_impl::resume_effect() const {
	if(current_effect)
		current_effect->resume();
}
void sound_impl::resume_interface_sound() const {
	if(current_interface_sound)
		current_interface_sound->resume();
}
void sound_impl::resume_music() const {
	auto const lm = last_music;
	if(lm != -1) {
		music_list[lm].resume();
	}
}

// called on startup and shutdown -- initialize should also load the list of available music files and load sound effects
void initialize_sound_system(sys::state& state) {
	state.sound_ptr = std::make_unique<sound_impl>();
	state.sound_ptr->window_handle = state.win_ptr->hwnd;

	auto root_dir = simple_fs::get_root(state.common_fs);
	auto const music_dir = simple_fs::open_directory(root_dir, NATIVE("music"));
	for(auto const& mp3_file : simple_fs::list_files(music_dir, NATIVE(".mp3"))) {
		auto file_name = simple_fs::get_full_name(mp3_file);
		state.sound_ptr->music_list.emplace_back(file_name);
		if(parsers::native_has_fixed_suffix_ci(file_name.c_str(), file_name.c_str() + file_name.length(), NATIVE("thecoronation_titletheme.mp3")))
			state.sound_ptr->first_music = int32_t(state.sound_ptr->music_list.size()) - 1;
	}
	for(auto const& music_subdir : simple_fs::list_subdirectories(music_dir)) {
		for(auto const& mp3_file : simple_fs::list_files(music_subdir, NATIVE(".mp3"))) {
			auto file_name = simple_fs::get_full_name(mp3_file);
			state.sound_ptr->music_list.emplace_back(file_name);
		}
	}

	struct {
		audio_instance* audio;
		native_string_view name;
	} vanilla_sound_table[] = {
		//{ &state.sound_ptr->click_sound, NATIVE("GI_ValidClick.wav") },
		{ &state.sound_ptr->technology_finished_sound, NATIVE("UI_TechnologyFinished.wav") },
		{ &state.sound_ptr->army_move_sound, NATIVE("GI_InfantryMove.wav") },
		{ &state.sound_ptr->army_select_sound, NATIVE("GI_InfantrySelected.wav") },
		{ &state.sound_ptr->navy_move_sound, NATIVE("UI_SailMove.wav") },
		{ &state.sound_ptr->navy_select_sound, NATIVE("UI_SailSelected.wav") },
		{ &state.sound_ptr->declaration_of_war_sound, NATIVE("DeclarationofWar.wav") },
		{ &state.sound_ptr->chat_message_sound, NATIVE("GI_ChatMessage.wav") },
		{ &state.sound_ptr->error_sound, NATIVE("GI_ErrorBlip.wav") },
		{ &state.sound_ptr->peace_sound, NATIVE("Misc_Peace.wav") },
		{ &state.sound_ptr->army_built_sound, NATIVE("UI_LandUnitFinished.wav") },
		{ &state.sound_ptr->navy_built_sound, NATIVE("UI_NavalUnitFinished.wav") },
		{ &state.sound_ptr->factory_built_sound, NATIVE("Misc_NewFactory.wav") },
		{ &state.sound_ptr->revolt_sound, NATIVE("Misc_revolt.wav") },
		{ &state.sound_ptr->fort_built_sound, NATIVE("Misc_Fortification.wav") },
		{ &state.sound_ptr->railroad_built_sound, NATIVE("Misc_Infrastructure.wav") },
		{ &state.sound_ptr->naval_base_built_sound, NATIVE("Misc_CoalingStation.wav") },
		{ &state.sound_ptr->minor_event_sound, NATIVE("GI_MinorBlip.wav") },
		{ &state.sound_ptr->major_event_sound, NATIVE("Misc_Attention.wav") },
		{ &state.sound_ptr->decline_sound, NATIVE("GI_FailureBlip.wav") },
		{ &state.sound_ptr->accept_sound, NATIVE("GI_SuccessBlip.wav") },
		{ &state.sound_ptr->diplomatic_request_sound, NATIVE("GI_MessageWindow.wav") },
		{ &state.sound_ptr->election_sound, NATIVE("Misc_ElectionHeld.wav") },
		{ &state.sound_ptr->land_battle_sounds[0], NATIVE("Combat_Cavalry_1.wav") },
		{ &state.sound_ptr->land_battle_sounds[1], NATIVE("Combat_Cavalry_2.wav") },
		{ &state.sound_ptr->land_battle_sounds[2], NATIVE("Combat_Cavalry_3.wav") },
		{ &state.sound_ptr->land_battle_sounds[3], NATIVE("Combat_Infantry_1.wav") },
		{ &state.sound_ptr->land_battle_sounds[4], NATIVE("Combat_Infantry_2.wav") },
		{ &state.sound_ptr->land_battle_sounds[5], NATIVE("Combat_Infantry_3.wav") },
		{ &state.sound_ptr->naval_battle_sounds[0], NATIVE("Combat_MajorShip_1.wav") },
		{ &state.sound_ptr->naval_battle_sounds[1], NATIVE("Combat_MajorShip_2.wav") },
		{ &state.sound_ptr->naval_battle_sounds[2], NATIVE("Combat_MajorShip_3.wav") },
		{ &state.sound_ptr->naval_battle_sounds[3], NATIVE("Combat_MinorShip_1.wav") },
		{ &state.sound_ptr->naval_battle_sounds[4], NATIVE("Combat_MinorShip_2.wav") },
		{ &state.sound_ptr->naval_battle_sounds[5], NATIVE("Combat_MinorShip_3.wav") },
	};
	auto const sound_directory = simple_fs::open_directory(root_dir, NATIVE("sound"));
	for(const auto& e : vanilla_sound_table) {
		auto file_peek = simple_fs::peek_file(sound_directory, e.name);
		e.audio->set_file(file_peek ? simple_fs::get_full_name(*file_peek) : native_string());
	}
	struct {
		audio_instance* audio;
		native_string_view name;
	} new_sound_table[] = {
		{ &state.sound_ptr->click_sound, NATIVE("NU_AltClick.wav") },
		{ &state.sound_ptr->click_left_sound, NATIVE("NU_ClickL.wav") },
		{ &state.sound_ptr->click_right_sound, NATIVE("NU_ClickR.wav") },
		{ &state.sound_ptr->console_open_sound, NATIVE("NU_OpenConsole.wav") },
		{ &state.sound_ptr->console_close_sound, NATIVE("NU_CloseConsole.wav") },
		{ &state.sound_ptr->tab_budget_sound, NATIVE("NU_TabBudget.wav") },
		{ &state.sound_ptr->tab_politics_sound, NATIVE("NU_TabPolitics.wav") },
		{ &state.sound_ptr->tab_diplomacy_sound, NATIVE("NU_TabDiplomacy.wav") },
		{ &state.sound_ptr->tab_military_sound, NATIVE("NU_TabMilitary.wav") },
		{ &state.sound_ptr->tab_population_sound, NATIVE("NU_TabPopulation.wav") },
		{ &state.sound_ptr->tab_production_sound, NATIVE("NU_TabProduction.wav") },
		{ &state.sound_ptr->tab_technology_sound, NATIVE("NU_TabTechnology.wav") },
		{ &state.sound_ptr->tab_military_sound, NATIVE("NU_TabMilitary.wav") },
		{ &state.sound_ptr->event_sound, NATIVE("NU_Event.wav") },
		{ &state.sound_ptr->decision_sound, NATIVE("NU_Decision.wav") },
		{ &state.sound_ptr->pause_sound, NATIVE("NU_Pause.wav") },
		{ &state.sound_ptr->unpause_sound, NATIVE("NU_Unpause.wav") },
		{ &state.sound_ptr->province_select_sounds[0], NATIVE("NU_ProvSelect1.wav") },
		{ &state.sound_ptr->province_select_sounds[1], NATIVE("NU_ProvSelect2.wav") },
		{ &state.sound_ptr->province_select_sounds[2], NATIVE("NU_ProvSelect3.wav") },
		{ &state.sound_ptr->province_select_sounds[3], NATIVE("NU_ProvSelect4.wav") },
	};
	auto const assets_directory = simple_fs::open_directory(root_dir, NATIVE("\\assets"));
	for(const auto& e : new_sound_table) {
		auto file_peek = simple_fs::peek_file(assets_directory, e.name);
		e.audio->set_file(file_peek ? simple_fs::get_full_name(*file_peek) : native_string());
	}
}

// these functions are called to change the volume of the currently playing track or effect
void change_effect_volume(sys::state& state, float v) {
	state.sound_ptr->change_effect_volume(v);
}
void change_interface_volume(sys::state& state, float v) {
	state.sound_ptr->change_interface_volume(v);
}
void change_music_volume(sys::state& state, float v) {
	state.sound_ptr->change_music_volume(v);
}

// these start playing an effect or track at the specified volume
void play_effect(sys::state& state, audio_instance& s, float volume) {
	if(volume > 0.0f)
		state.sound_ptr->play_effect(s, volume);
}
void play_interface_sound(sys::state& state, audio_instance& s, float volume) {
	if(volume > 0.0f)
		state.sound_ptr->play_interface_sound(s, volume);
}

// controls autoplaying music (start music should start with the main theme)
void stop_music(sys::state& state) {
	if(state.sound_ptr->last_music != -1)
		state.sound_ptr->music_list[state.sound_ptr->last_music].stop();
	state.sound_ptr->last_music = -1;
}
void start_music(sys::state& state, float v) {
	if(v > 0.0f && state.sound_ptr->music_list.size() != 0) {
		if(state.sound_ptr->first_music != -1)
			state.sound_ptr->play_music(state.sound_ptr->first_music, v);
		else
			state.sound_ptr->play_music(int32_t(rand() % state.sound_ptr->music_list.size()), v);
	}
}

void pause_all(sys::state& state) {
	if(!state.sound_ptr.get())
		return;

	state.sound_ptr->global_pause = true;
	state.sound_ptr->pause_effect();
	state.sound_ptr->pause_interface_sound();
	state.sound_ptr->pause_music();
}
void resume_all(sys::state& state) {
	if(!state.sound_ptr.get())
		return;

	state.sound_ptr->global_pause = false;
	state.sound_ptr->resume_effect();
	state.sound_ptr->resume_interface_sound();
	state.sound_ptr->resume_music();
}

void update_music_track(sys::state& state) {
	if(state.sound_ptr->music_finished())
		state.sound_ptr->play_new_track(state);
}

// returns the default click sound -- expect this list of functions to expand as
//    we implement more of the fixed sound effects
audio_instance& get_click_sound(sys::state& state) {
	return state.sound_ptr->click_sound;
}
audio_instance& get_click_left_sound(sys::state& state) {
	return state.sound_ptr->click_left_sound;
}
audio_instance& get_click_right_sound(sys::state& state) {
	return state.sound_ptr->click_right_sound;
}
audio_instance& get_tab_budget_sound(sys::state& state) {
	return state.sound_ptr->tab_budget_sound;
}
audio_instance& get_tab_politics_sound(sys::state& state) {
	return state.sound_ptr->tab_politics_sound;
}
audio_instance& get_tab_diplomacy_sound(sys::state& state) {
	return state.sound_ptr->tab_diplomacy_sound;
}
audio_instance& get_tab_military_sound(sys::state& state) {
	return state.sound_ptr->tab_military_sound;
}
audio_instance& get_tab_population_sound(sys::state& state) {
	return state.sound_ptr->tab_population_sound;
}
audio_instance& get_tab_production_sound(sys::state& state) {
	return state.sound_ptr->tab_production_sound;
}
audio_instance& get_tab_technology_sound(sys::state& state) {
	return state.sound_ptr->tab_technology_sound;
}
audio_instance& get_army_select_sound(sys::state& state) {
	return state.sound_ptr->army_select_sound;
}
audio_instance& get_army_move_sound(sys::state& state) {
	return state.sound_ptr->army_move_sound;
}
audio_instance& get_navy_select_sound(sys::state& state) {
	return state.sound_ptr->navy_select_sound;
}
audio_instance& get_navy_move_sound(sys::state& state) {
	return state.sound_ptr->navy_move_sound;
}
audio_instance& get_error_sound(sys::state& state) {
	return state.sound_ptr->error_sound;
}
audio_instance& get_peace_sound(sys::state& state) {
	return state.sound_ptr->peace_sound;
}
audio_instance& get_army_built_sound(sys::state& state) {
	return state.sound_ptr->army_built_sound;
}
audio_instance& get_navy_built_sound(sys::state& state) {
	return state.sound_ptr->navy_built_sound;
}
audio_instance& get_declaration_of_war_sound(sys::state& state) {
	return state.sound_ptr->declaration_of_war_sound;
}
audio_instance& get_technology_finished_sound(sys::state& state) {
	return state.sound_ptr->technology_finished_sound;
}
audio_instance& get_factory_built_sound(sys::state& state) {
	return state.sound_ptr->factory_built_sound;
}
audio_instance& get_election_sound(sys::state& state) {
	return state.sound_ptr->election_sound;
}
audio_instance& get_revolt_sound(sys::state& state) {
	return state.sound_ptr->revolt_sound;
}
audio_instance& get_fort_built_sound(sys::state& state) {
	return state.sound_ptr->fort_built_sound;
}
audio_instance& get_railroad_built_sound(sys::state& state) {
	return state.sound_ptr->railroad_built_sound;
}
audio_instance& get_naval_base_built_sound(sys::state& state) {
	return state.sound_ptr->naval_base_built_sound;
}
audio_instance& get_minor_event_sound(sys::state& state) {
	return state.sound_ptr->minor_event_sound;
}
audio_instance& get_major_event_sound(sys::state& state) {
	return state.sound_ptr->major_event_sound;
}
audio_instance& get_decline_sound(sys::state& state) {
	return state.sound_ptr->decline_sound;
}
audio_instance& get_accept_sound(sys::state& state) {
	return state.sound_ptr->accept_sound;
}
audio_instance& get_diplomatic_request_sound(sys::state& state) {
	return state.sound_ptr->diplomatic_request_sound;
}
audio_instance& get_chat_message_sound(sys::state& state) {
	return state.sound_ptr->chat_message_sound;
}
audio_instance& get_console_open_sound(sys::state& state) {
	return state.sound_ptr->console_open_sound;
}
audio_instance& get_console_close_sound(sys::state& state) {
	return state.sound_ptr->console_close_sound;
}

audio_instance& get_event_sound(sys::state& state) {
	return state.sound_ptr->event_sound;
}
audio_instance& get_decision_sound(sys::state& state) {
	return state.sound_ptr->decision_sound;
}
audio_instance& get_pause_sound(sys::state& state) {
	return state.sound_ptr->pause_sound;
}
audio_instance& get_unpause_sound(sys::state& state) {
	return state.sound_ptr->unpause_sound;
}

audio_instance& get_random_land_battle_sound(sys::state& state) {
	return state.sound_ptr->land_battle_sounds[int32_t(std::rand() % 6)];
}
audio_instance& get_random_naval_battle_sound(sys::state& state) {
	return state.sound_ptr->naval_battle_sounds[int32_t(std::rand() % 6)];
}
audio_instance& get_random_province_select_sound(sys::state& state) {
	return state.sound_ptr->province_select_sounds[int32_t(std::rand() % 4)];
}

void play_new_track(sys::state& state) {
	state.sound_ptr->play_new_track(state);
}
void play_next_track(sys::state& state) {
	state.sound_ptr->play_next_track(state);
}
void play_previous_track(sys::state& state) {
	state.sound_ptr->play_previous_track(state);
}

native_string get_current_track_name(sys::state& state) {
	if(state.sound_ptr->last_music == -1)
		return NATIVE("");
	return state.sound_ptr->music_list[state.sound_ptr->last_music].filename;
}

} // namespace sound
