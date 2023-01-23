#include "system_state.hpp"
#include "opengl_wrapper.hpp"
#include <cassert>

#ifdef _WIN64
#include "window_win.cpp"
#else
#include "window_nix.cpp"
#endif

namespace sys {
	state::~state() {
		// why does this exist ? So that the destructor of the unique pointer doesn't have to be known before it is implemented
	}

	void state::on_rbutton_down(int32_t x, int32_t y, key_modifiers mod) {
		window::set_borderless_full_screen(*this, true);
	}
	void state::on_lbutton_down(int32_t x, int32_t y, key_modifiers mod) {
		window::set_borderless_full_screen(*this, false);
	}
	void state::on_rbutton_up(int32_t x, int32_t y, key_modifiers mod) {

	}
	void state::on_lbutton_up(int32_t x, int32_t y, key_modifiers mod) {

	}
	void state::on_mouse_move(int32_t x, int32_t y, key_modifiers mod) {

	}
	void state::on_mouse_drag(int32_t x, int32_t y, key_modifiers mod) { // called when the left button is held down
	
	}
	void state::on_resize(int32_t x, int32_t y, window_state win_state) {

	}
	void state::on_mouse_wheel(int32_t x, int32_t y, key_modifiers mod, float amount) { // an amount of 1.0 is one "click" of the wheel
	
	}
	void state::on_key_down(virtual_key keycode, key_modifiers mod) {

	}
	void state::on_key_up(virtual_key keycode, key_modifiers mod) {

	}
	void state::on_text(char c) { // c is win1250 codepage value

	}
	void state::render() { // called to render the frame may (and should) delay returning until the frame is rendered, including waiting for vsync
		// assert(opengl_context != nullptr);
		glClearColor(1.0, 0.0, 1.0, 0.5);
		glClear(GL_COLOR_BUFFER_BIT);
	}
}