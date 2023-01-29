## Sound system

The sound system is a relatively uninteresting part of the project. We divide all sounds into three categories. Music, interface sounds (for example mouse clicks), and sound effects (none currently implemented). Music files play continuously in a random order when their effective volume (master volume x music volume) is non-negative. Interface sounds play only once, and only one interface sound can be playing at a time. If you try to play an interface sound while another is still playing, the interface sound that is currently playing will be cancelled. Sound effects work much like interface sounds. Like them, there can only be one playing at a time. However, if you try to play a sound effect while another sound effect is still playing, the previous sound effect will continue to play, and the new sound effect will not play.

All volumes in the sound system are expressed as floating point values in the range 0.0 to 1.0. There is a stored volume setting for each category of sound, as well as a master volume setting. When calculating the volume to play an sound from a particular category, the volume for that category is multiplied with the master volume to determine the volume that it will play at.

### Initialization

The sound system is initialized with `void initialize_sound_system(sys::state& state)`. This function is responsible for creating the platform dependent `sound_impl` object (stored in the `sound_ptr` member of the `sys::state` object) and for enumerating the music files. This function should be called somewhere within the window creation routine.

### Playing interface sounds and sound effects

The `void play_effect(sys::state& state, audio_instance& s, float volume)` and `void play_interface_sound(sys::state& state, audio_instance& s, float volume)` functions are responsible for playing sound effects and interface sounds. Their volume parameter ranges from 0.0 to 1.0 (and is calculated by multiplying the master volume from user settings with either the interface sound volume or sound effect volume saved there). These functions take care of detecting whether other sounds are playing and reacting appropriately. Note that both these functions identify which sound to play via the implementation-defined `audio_instance` structure. At the moment, the only instance of such a structure that is provided is through the `audio_instance& get_click_sound(sys::state& state)` function, which returns the sound that plays when a button is clicked.

### Playing music

Music is controlled via the `void stop_music(sys::state& state)` and `void start_music(sys::state& state, float volume)` functions. These start and stop the auto playing music loop (assuming that the effective volume is > 0.0; as an optimization I don't play the music at all when there is no volume). Due to the way my implementation was written I needed a function to call back into the sound system with when the currently playing music ended, which I exposed as `void update_music_track(sys::state& state)`. However, this function should not be officially relied on to do anything. *It has no platform-independent meaning.* Also, when the music begins playing, the windows implementation always starts with `thecoronation_titletheme.mp3`.

### Changing volume

There are three functions for changing the volume:

- `void change_effect_volume(sys::state& state, float volume)`
- `void change_interface_volume(sys::state& state, float volume)`
- `void change_music_volume(sys::state& state, float volume)`

These functions *do not* change the volume settings in any way. Rather, they change the volume of any currently playing sound or music in their respective category. When the user changes a sound setting, we call these functions to make sure that the change is immediately reflected in any playing sound.
