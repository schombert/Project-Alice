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
			MessageBoxW(nullptr, L"failed to create graph builder", L"Audio error", MB_OK);
			std::abort();
		}

		std::thread creation_dispatch([pGraph, as_music, volume, window_handle, _this = this]() {
			HRESULT hr = pGraph->RenderFile((wchar_t const*)(_this->filename.c_str()), nullptr);
			if(FAILED(hr)) {
				MessageBoxW(nullptr, L"unable to play audio file", L"Audio error", MB_OK);
				_this->volume_multiplier = 0.0f;
				return;
			}

			IMediaControl* pControl = nullptr;
			hr = pGraph->QueryInterface(IID_IMediaControl, (void**)&pControl);

			if(FAILED(hr)) {
				MessageBoxW(nullptr, L"failed to get control interface", L"Audio error", MB_OK);
				std::abort();
			}

			if(as_music) {
				IMediaEventEx* pEvent = nullptr;
				hr = pGraph->QueryInterface(IID_IMediaEventEx, (void**)&pEvent);

				if(FAILED(hr)) {
					MessageBoxW(nullptr, L"failed to get event interface", L"Audio error", MB_OK);
					std::abort();
				}
				auto const res_b = pEvent->SetNotifyWindow((OAHWND)window_handle, WM_GRAPHNOTIFY, NULL);
				if(FAILED(res_b)) {
					MessageBoxW(nullptr, L"failed to set notification window", L"Audio error", MB_OK);
					std::abort();
				}
				_this->event_interface = pEvent;
			}

			IBasicAudio* pAudio = nullptr;
			hr = pGraph->QueryInterface(IID_IBasicAudio, (void**)&pAudio);

			if(FAILED(hr)) {
				MessageBoxW(nullptr, L"failed to get audio interface", L"Audio error", MB_OK);
				std::abort();
			}

			IMediaSeeking* pSeek = nullptr;
			hr = pGraph->QueryInterface(IID_IMediaSeeking, (void**)&pSeek);

			if(FAILED(hr)) {
				MessageBoxW(nullptr, L"failed to get seeking interface", L"Audio error", MB_OK);
				std::abort();
			}

			_this->graph_interface = pGraph;
			_this->control_interface = pControl;
			_this->audio_interface = pAudio;
			_this->seek_interface = pSeek;

			hr = ((IBasicAudio*)pAudio)->put_Volume(volume_function(volume * _this->volume_multiplier));
			if(FAILED(hr)) {
				MessageBoxW(nullptr, L"failed to put_Volume", L"Audio error", MB_OK);
			}

			LONGLONG new_position = 0;
			hr =
					((IMediaSeeking*)pSeek)->SetPositions(&new_position, AM_SEEKING_AbsolutePositioning, nullptr, AM_SEEKING_NoPositioning);
			if(FAILED(hr)) {
				MessageBoxW(nullptr, L"failed to SetPositions", L"Audio error", MB_OK);
			}
			hr = ((IMediaControl*)pControl)->Run();
			if(FAILED(hr)) {
				MessageBoxW(nullptr, L"failed to Run", L"Audio error", MB_OK);
			}
		});
		creation_dispatch.detach();
	} else {
		HRESULT hr;
		if(audio_interface) {
			hr = audio_interface->put_Volume(volume_function(volume * volume_multiplier));
			if(FAILED(hr)) {
				MessageBoxW(nullptr, L"failed to put_Volume", L"Audio error", MB_OK);
			}
		}
		if(seek_interface) {
			LONGLONG new_position = 0;
			hr = seek_interface->SetPositions(&new_position, AM_SEEKING_AbsolutePositioning, nullptr, AM_SEEKING_NoPositioning);
			if(FAILED(hr)) {
				MessageBoxW(nullptr, L"failed to SetPositions", L"Audio error", MB_OK);
			}
		}

		if(control_interface) {
			hr = control_interface->Run();
			if(FAILED(hr)) {
				MessageBoxW(nullptr, L"failed to Run", L"Audio error", MB_OK);
			}
		}
	}
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
		while(result == last_music)
			result = int32_t(rand() % music_list.size());
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
	if(!current_effect || current_effect->is_playing() == false) {
		s.play(volume, false, window_handle);
		current_effect = &s;
	}
}

void sound_impl::play_interface_sound(audio_instance& s, float volume) {
	if(current_interface_sound)
		current_interface_sound->stop();
	current_interface_sound = &s;
	s.play(volume, false, window_handle);
}

void sound_impl::play_music(int32_t track, float volume) {
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

// called on startup and shutdown -- initialize should also load the list of available music files and load sound effects
void initialize_sound_system(sys::state& state) {
	state.sound_ptr = std::make_unique<sound_impl>();
	auto root_dir = get_root(state.common_fs);
	auto const music_directory = open_directory(root_dir, NATIVE("music"));

	state.sound_ptr->window_handle = state.win_ptr->hwnd;

	for(auto const& mp3_file : list_files(music_directory, NATIVE(".mp3"))) {
		auto file_name = get_full_name(mp3_file);
		state.sound_ptr->music_list.emplace_back(file_name);
		if(parsers::native_has_fixed_suffix_ci(file_name.c_str(), file_name.c_str() + file_name.length(),
			NATIVE("thecoronation_titletheme.mp3")))
			state.sound_ptr->first_music = int32_t(state.sound_ptr->music_list.size()) - 1;
	}

	auto const sound_directory = open_directory(root_dir, NATIVE("\\sound"));

	{
		auto file_peek = peek_file(sound_directory, NATIVE("GI_ValidClick.wav"));
		state.sound_ptr->click_sound.set_file(file_peek ? get_full_name(*file_peek) : std::wstring());
	}
	{
		auto file_peek = peek_file(sound_directory, NATIVE("UI_TechnologyFinished.wav"));
		state.sound_ptr->technology_finished_sound.set_file(file_peek ? get_full_name(*file_peek) : std::wstring());
	}
	{
		auto file_peek = peek_file(sound_directory, NATIVE("GI_InfantryMove.wav"));
		state.sound_ptr->army_move_sound.set_file(file_peek ? get_full_name(*file_peek) : std::wstring());
	}
	{
		auto file_peek = peek_file(sound_directory, NATIVE("GI_InfantrySelected.wav"));
		state.sound_ptr->army_select_sound.set_file(file_peek ? get_full_name(*file_peek) : std::wstring());
	}
	{
		auto file_peek = peek_file(sound_directory, NATIVE("UI_SailMove.wav"));
		state.sound_ptr->navy_move_sound.set_file(file_peek ? get_full_name(*file_peek) : std::wstring());
	}
	{
		auto file_peek = peek_file(sound_directory, NATIVE("UI_SailSelected.wav"));
		state.sound_ptr->navy_select_sound.set_file(file_peek ? get_full_name(*file_peek) : std::wstring());
	}
	{
		auto file_peek = peek_file(sound_directory, NATIVE("DeclarationofWar.wav"));
		state.sound_ptr->declaration_of_war_sound.set_file(file_peek ? get_full_name(*file_peek) : std::wstring());
	}
	{
		auto file_peek = peek_file(sound_directory, NATIVE("GI_ChatMessage.wav"));
		state.sound_ptr->chat_message_sound.set_file(file_peek ? get_full_name(*file_peek) : std::wstring());
	}
	{
		auto file_peek = peek_file(sound_directory, NATIVE("GI_ErrorBlip.wav"));
		state.sound_ptr->error_sound.set_file(file_peek ? get_full_name(*file_peek) : std::wstring());
	}
	{
		auto file_peek = peek_file(sound_directory, NATIVE("Misc_Peace.wav"));
		state.sound_ptr->peace_sound.set_file(file_peek ? get_full_name(*file_peek) : std::wstring());
	}
	{
		auto file_peek = peek_file(sound_directory, NATIVE("UI_LandUnitFinished.wav"));
		state.sound_ptr->army_built_sound.set_file(file_peek ? get_full_name(*file_peek) : std::wstring());
	}
	{
		auto file_peek = peek_file(sound_directory, NATIVE("UI_NavalUnitFinished.wav"));
		state.sound_ptr->navy_built_sound.set_file(file_peek ? get_full_name(*file_peek) : std::wstring());
	}
	{
		auto file_peek = peek_file(sound_directory, NATIVE("Misc_NewFactory.wav"));
		state.sound_ptr->factory_built_sound.set_file(file_peek ? get_full_name(*file_peek) : std::wstring());
	}
	{
		auto file_peek = peek_file(sound_directory, NATIVE("Misc_revolt.wav"));
		state.sound_ptr->revolt_sound.set_file(file_peek ? get_full_name(*file_peek) : std::wstring());
	}
	{
		auto file_peek = peek_file(sound_directory, NATIVE("Misc_Fortification.wav"));
		state.sound_ptr->fort_built_sound.set_file(file_peek ? get_full_name(*file_peek) : std::wstring());
	}
	{
		auto file_peek = peek_file(sound_directory, NATIVE("Misc_Infrastructure.wav"));
		state.sound_ptr->railroad_built_sound.set_file(file_peek ? get_full_name(*file_peek) : std::wstring());
	}
	{
		auto file_peek = peek_file(sound_directory, NATIVE("Misc_CoalingStation.wav"));
		state.sound_ptr->naval_base_built_sound.set_file(file_peek ? get_full_name(*file_peek) : std::wstring());
	}
	{
		auto file_peek = peek_file(sound_directory, NATIVE("GI_MinorBlip.wav"));
		state.sound_ptr->minor_event_sound.set_file(file_peek ? get_full_name(*file_peek) : std::wstring());
	}
	{
		auto file_peek = peek_file(sound_directory, NATIVE("Misc_Attention.wav"));
		state.sound_ptr->major_event_sound.set_file(file_peek ? get_full_name(*file_peek) : std::wstring());
	}
	{
		auto file_peek = peek_file(sound_directory, NATIVE("GI_FailureBlip.wav"));
		state.sound_ptr->decline_sound.set_file(file_peek ? get_full_name(*file_peek) : std::wstring());
	}
	{
		auto file_peek = peek_file(sound_directory, NATIVE("GI_SuccessBlip.wav"));
		state.sound_ptr->accept_sound.set_file(file_peek ? get_full_name(*file_peek) : std::wstring());
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

void update_music_track(sys::state& state) {
	if(state.sound_ptr->music_finished())
		state.sound_ptr->play_new_track(state);
}

// returns the default click sound -- expect this list of functions to expand as
//    we implement more of the fixed sound effects
audio_instance& get_click_sound(sys::state& state) {
	return state.sound_ptr->click_sound;
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

} // namespace sound
