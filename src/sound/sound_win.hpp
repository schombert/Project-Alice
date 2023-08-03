#pragma once

struct IGraphBuilder;
struct IMediaControl;
struct IBasicAudio;
struct IMediaSeeking;
struct IMediaEventEx;
typedef struct HWND__* HWND;

namespace sound {

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
	audio_instance(std::wstring const& file) : filename(file) { }
	audio_instance(audio_instance const&) = delete;
	audio_instance(audio_instance&& o) noexcept
		: filename(std::move(o.filename)), graph_interface(o.graph_interface), control_interface(o.control_interface), audio_interface(o.audio_interface), seek_interface(o.seek_interface), event_interface(o.event_interface), volume_multiplier(o.volume_multiplier) {

		o.graph_interface = nullptr;
		o.control_interface = nullptr;
		o.audio_interface = nullptr;
		o.seek_interface = nullptr;
		o.event_interface = nullptr;
	}
	~audio_instance();

	void set_file(std::wstring const& file) { filename = file; }
	void play(float volume, bool as_music, void* window_handle);
	void stop() const;
	bool is_playing() const;
	void change_volume(float new_volume) const;

	friend class sound_impl;
};

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

	void play_new_track(sys::state& ws);
};

} // namespace sound
