#include "sound.hpp"
#include "system_state.hpp"
#include "Dshow.h"

#define WM_GRAPHNOTIFY (WM_APP + 1)

#pragma comment(lib, "Strmiids.lib")

namespace sound {

constexpr int32_t volume_function(float v) {
	return std::clamp(int32_t((v + -1.0f) * 4'500.0f), -10'000, 0);
}

class audio_instance {
private:
	std::wstring filename;
	IGraphBuilder* graph_interface = nullptr;
	IMediaControl* control_interface = nullptr;
	IBasicAudio* audio_interface = nullptr;
	IMediaSeeking* seek_interface = nullptr;
	IMediaEventEx* event_interface = nullptr;

public:
	float volume_multiplier = 1.0f;

	audio_instance() { }
	audio_instance(std::wstring const & file) : filename(file) { }
	audio_instance(audio_instance const &) = delete;
	audio_instance(audio_instance&& o) noexcept : filename(std::move(o.filename)), graph_interface(o.graph_interface),
	                                              control_interface(o.control_interface), audio_interface(o.audio_interface), seek_interface(o.seek_interface),
	                                              event_interface(o.event_interface), volume_multiplier(o.volume_multiplier) {

		o.graph_interface = nullptr;
		o.control_interface = nullptr;
		o.audio_interface = nullptr;
		o.seek_interface = nullptr;
		o.event_interface = nullptr;
	}
	~audio_instance() {
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

	void set_file(std::wstring const & file) {
		filename = file;
	}
	void play(float volume, bool as_music, void* window_handle);
	void stop() const;
	bool is_playing() const;
	void change_volume(float new_volume) const;

	friend class sound_impl;
};

void audio_instance::play(float volume, bool as_music, void* window_handle) {
	if(volume * volume_multiplier <= 0.0f || filename.size() == 0)
		return;

	if(!graph_interface) {
		IGraphBuilder* pGraph = nullptr;

		HRESULT hr = CoCreateInstance(CLSID_FilterGraph, nullptr,
		                              CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&pGraph);
		if(FAILED(hr)) {
			MessageBoxW(nullptr, L"failed to create graph builder", L"Audio error", MB_OK);
			std::abort();
		}

		std::thread creation_dispatch([pGraph, as_music, volume, window_handle, _this = this]() {
			HRESULT hr = pGraph->RenderFile((wchar_t*)(_this->filename.c_str()), nullptr);
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
			hr = ((IMediaSeeking*)pSeek)->SetPositions(&new_position, AM_SEEKING_AbsolutePositioning, nullptr, AM_SEEKING_NoPositioning);
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

class sound_impl {
private:
	audio_instance* current_effect = nullptr;
	audio_instance* current_interface_sound = nullptr;

public:
	HWND window_handle = nullptr;
	int32_t last_music = -1;
	int32_t first_music = -1;

	audio_instance click_sound;

	std::vector<audio_instance> music_list;

	void play_effect(audio_instance& s, float volume);
	void play_interface_sound(audio_instance& s, float volume);
	void play_music(int32_t track, float volume);

	void change_effect_volume(float v) const;
	void change_interface_volume(float v) const;
	void change_music_volume(float v) const;

	bool music_finished() const;

	void play_new_track(sys::state& ws) {
		if(music_list.size() > 0) {
			int32_t result = int32_t(rand() % music_list.size()); // well aware that using rand is terrible, thanks
			while(result == last_music)
				result = int32_t(rand() % music_list.size());
			play_music(result, ws.user_settings.master_volume * ws.user_settings.music_volume);
		}
	}
};

bool sound_impl::music_finished() const {
	const auto lm = last_music;
	if(lm == -1)
		return false;

	long evCode;
	LONG_PTR param1, param2;
	const auto event_interface = music_list[lm].event_interface;
	if(event_interface) {
		while(SUCCEEDED(event_interface->GetEvent(&evCode, &param1, &param2, 0))) {
			event_interface->FreeEventParams(evCode, param1, param2);
			switch(evCode) {
			case EC_COMPLETE:  // Fall through.
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
	const auto lm = last_music;
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
	const auto lm = last_music;
	if(lm != -1) {
		music_list[lm].change_volume(v);
	}
}

// called on startup and shutdown -- initialize should also load the list of available music files and load sound effects
void initialize_sound_system(sys::state& state) {
	state.sound_ptr = std::make_unique<sound_impl>();
	auto root_dir = get_root(state.common_fs);
	const auto music_directory = open_directory(root_dir, NATIVE("music"));

	state.sound_ptr->window_handle = state.win_ptr->hwnd;

	for(const auto& mp3_file : list_files(music_directory, NATIVE(".mp3"))) {
		auto file_name = get_full_name(mp3_file);
		state.sound_ptr->music_list.emplace_back(file_name);
		if(parsers::native_has_fixed_suffix_ci(file_name.c_str(), file_name.c_str() + file_name.length(), NATIVE("thecoronation_titletheme.mp3")))
			state.sound_ptr->first_music = int32_t(state.sound_ptr->music_list.size()) - 1;
	}

	const auto sound_directory = open_directory(root_dir, NATIVE("\\sound"));
	auto click_peek = peek_file(sound_directory, NATIVE("GI_ValidClick.wav"));

	state.sound_ptr->click_sound.set_file(click_peek ? get_full_name(*click_peek) : std::wstring());
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

// returns the default click sound -- expect this list of functions to expand as
//    we implement more of the fixed sound effects
audio_instance& get_click_sound(sys::state& state) {
	return state.sound_ptr->click_sound;
}

void update_music_track(sys::state& state) {
	if(state.sound_ptr->music_finished())
		state.sound_ptr->play_new_track(state);
}

} // namespace sound
