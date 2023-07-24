## The window wrapper

The window wrapper is defined in the namespace `window`, and its few public interfaces are declared in the file `window.hpp`.

There are only five functions that allow you to interact with a window on your terms (all other interactions with the window will call from it calling functions to signal that some event has occurred). These functions are as follows:

- `void create_window(sys::state& game_state, creation_parameters const& params);` -- `window::creation_parameters` is used nowhere except this function. It is simply a convenience struct to better order the parameters. These parameters are as follows:
```
struct creation_parameters {
	int32_t size_x;
	int32_t size_y;
	sys::window_state intitial_state;
	bool borderless_fullscreen;
};
```
The `size_x` and `size_y` parameters define the default size for the window when it is not maximized. `intitial_state` must be one of `sys::window_state::normal`, `sys::window_state::normal`, or `sys::window_state::minimized`, which will cause the window to start in the specified state *unless* `borderless_fullscreen` is true. If `borderless_fullscreen` is true, the window will start in that mode regardless of the other settings. However, you should still keep track of them, in case the user exits borderless full screen so that you have something sensible to fall back to.
- `void close_window(sys::state& game_state)` -- calling this function will close the main window. It should only be called form the context of the ui and rendering thread, so there should be no synchronization issues. While we aren't quite there yet, calling this function should also send the appropriate signal to the game state to stop running.
- `void set_borderless_full_screen(sys::state& game_state, bool fullscreen)` -- also called only from the ui and rendering thread, this function changes whether the window is in borderless full screen mode.
- `bool is_in_fullscreen(sys::state const& game_state)` -- returns whether the window is in the full screen mode. I don't know if we will ever need this, but it is nice to have just the same.
- `bool is_key_depressed(sys::state const& game_state, sys::virtual_key key)` -- returns whether the key, as named by the `sys::virtual_key` enumeration is currently being held down. Once again, this will only be called from the ui and rendering thread.
- `void emit_error_message(std::string const& content, bool fatal)` -- emits an error message and terminates if `fatal` is true.

### Threading policy

The current plan is the following: there should be a single thread that both dispatches messages from the window subsystem and which renders the map and ui. Thus rendering should never be happening while changes are being made to the ui or vice versa. Nor should it be possible for two events from the window subsystem to occur simultaneously. The game state itself will update on a completely disjoint set of threads. While the ui and rendering threads will be able to read data from the game state without blocking, any commands to change the game state from the ui will have to be placed in some form of lock-free queue or signal it by some other safe mechanism. The key advantage of this setup is that it makes writing the ui much easier, since it allows the ui to be written as if were in a single-threaded program.

### The system state header

`system_state.hpp` defines the main object of the game while will eventually be the repository of essentially everything going on in the game. A reference to this will essentially be visible everywhere in the ui, allowing a ui command to easily do things ranging from reading the game state to changing the music. It holds some data that represents the visible state of the window subsystem that any implementation is responsible for updating as needed. Specifically it holds:
- `int32_t x_size` -- the current horizontal size of the window in pixels
- `int32_t y_size` -- the current vertical size of the window in pixels
- `int32_t mouse_x_position` -- the previous x position, in pixels, within the client area of the window that the mouse was last seen at.
- `int32_t mouse_y_position` -- the previous y position, in pixels, within the client area of the window that the mouse was last seen at.
It also contains the boolean value
`bool in_edit_control` that indicates whether a control expecting character input has the focus. You should only raise `on_text` events (see below) when this is true.

### UI Events

The `state` object is also home to a number of event functions that the window subsystem is expected to raise. (The implementations of these events are found in `system_state.cpp`, and represent the ui handling logic that is common to all platforms; there are no platform dependent versions of them.)

Many of these events take a `sys::key_modifiers` enum as a parameter. This enumeration reports the state of the `alt`, `ctrl`, and `shift` keys at the time that the event was raised.

First we have the mouse messages:
- `void on_rbutton_down(int32_t x, int32_t y, key_modifiers mod)`
- `void on_lbutton_down(int32_t x, int32_t y, key_modifiers mod)`
- `void on_rbutton_up(int32_t x, int32_t y, key_modifiers mod)`
- `void on_lbutton_up(int32_t x, int32_t y, key_modifiers mod)`
- `void on_mouse_move(int32_t x, int32_t y, key_modifiers mod)`
- `void on_mouse_drag(int32_t x, int32_t y, key_modifiers mod)`
- `void on_mouse_wheel(int32_t x, int32_t y, key_modifiers mod, float amount)`
Each of these functions also receives the current x and y position of the mouse at the time when the event is raised. **Note** the stored mouse position in the `state` object should not be updated until after these functions have returned, which allows the previous position of the mouse to be inspected, if necessary. `mouse_drag` should be produced when the mouse moves while the left mouse button is held down. This message should be produced *in addition to* the `on_mouse_move` message, and should be raised after it. Finally, each `1.0f` in the `amount` parameter should correspond to about one "click" of the mouse wheel. I don't know what direction positive should be. If you notice it going in the "wrong" direction in your implementation, just flip the sign. I will do the same, and hopefully that will bring us into agreement.

The resize message:
- `void on_resize(int32_t x, int32_t y, window_state win_state)` -- called when the size of the window changes. As with the mouse event, update the stored window size after the message returns in case the recipient needed to compare with the old size. This event also signals when the window changes from one of being maximized, normal, or minimized to another one of those states. The `win_state` contains the state that the window will be transitioning into (or remaining in, if it is just being resized). You should automatically adjust the OpenGL viewport before raising this message, as no viewport size management is done in the platform-independent parts of the code.

Keyboard messages:
- `void on_key_down(virtual_key keycode, key_modifiers mod)` -- Raised when a key is first depressed (no auto-repeat messages, if your OS sends those, should be passed on). This message is sent for all keys, even system keys such as `alt`, but a `key_modifiers` enum is still passed keeping track of the system key state as a whole. Now, about that `keycode`. Although this is an enum defines in the `system_state.hpp` file, its values agree with the standard virtual key mappings, which made my life very easy. Obviously, there will be some annoyance in the need to write a function mapping these back and forth to whatever virtual key system Linux uses (we need an enum -> Linux-native function so that we can ask which key is held down in a platform-independent way as well). Since I made this choice, it is only fair that I write these two irritating functions. So all I ask is that you write the necessary stubs for them and then point me to the necessary Linux documentation with all the values, and I will do the rest.
` void on_key_up(virtual_key keycode, key_modifiers mod)` -- Raised when a key ceases being held down (no danger of repeats here).
- `void on_text(char c)` -- This event should be raised only when `in_edit_control` is set. Even when you are sending these messages, the `on_key_down` and `on_key_up` events should continue to be raised. The parameter `c` should be a win1250 codepage value, which will require some translation from however your system normally communicates text input (you can fall back to just sending ASCII input as a simple first draft). The reason for this choice is two-fold. First: by keeping this text in the same codepage as the rest of the text in the game, it allows for a single rendering path for it and makes string comparisons easy. Second: since it is native to neither Windows nor Linux, it serves as a common standard that both implementations can target. If there is no appropriate win1250 codepage value, then no message should be sent.

The `render` message:
- `void render()` -- is the final of the message functions that the window subsystem needs to call. It isn't really a "message" exactly, but it needs to be scheduled with the messages on the same thread, and so the window subsystem is expected to call `render` at appropriate times in between messages. It is also expected to do the Linux equivalent of `SwapBuffers` (i.e. to wait until vsync and send the frame to be rendered -- the waiting part is important as we don't want to be sending more frames than the monitor can render, as that simply wastes CPU resources that could be better spent elsewhere).

### Interaction with OpenGL

There is still much to be done for the OpenGL subsystem, but there are two simple pieces that the windowing subsystem is responsible (in addition to updating the viewport, as mentioned above).  The `opengl_wrapper` platform implementation file needs to define `void create_opengl_context(sys::state& state)` and `void shutdown_opengl(sys::state& state)`. `create_opengl_context` should create the OpenGL context, attach it to the window, and do any other platform-specific setup work. Similarly, `shutdown_opengl` should do any platform-specific work required to tear down OpenGL.

The windowing system is responsible for calling `intialize_opengl` (**Note** not the same thing as `create_opengl_context`) at some appropriate point. This function calls the platform specific `create_opengl_context` function and then does any platform-independent OpenGL initialization we want. The windowing subsystem is also responsible for calling `shutdown_opengl` when the window is closing.

### Implementation details: `window::window_data_impl` 

The `sys::state` object contains a `unique_ptr` holding a `window::window_data_impl`. The contents of this `class` can be defined as you see fit to hold any information the platform-dependent code uses to manage the window. You are responsible for creating and storing an instance of this `unique_ptr` in your `create_window` implementation.