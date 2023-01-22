#include "system_state.hpp"
#include "opengl_wrapper.hpp"
#include "window.hpp"
#include <algorithm>
#include <functional>

namespace sys {
	//
	// window event functions
	//

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
		glClearColor(1.0, 0.0, 1.0, 0.5);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	//
	// string pool functions
	//

	std::string_view state::to_string_view(text_tag tag) const {
		return std::string_view(text_data.data() + tag.start.index(), tag.length);
	}

	text_tag state::add_to_pool(std::string_view text) {
		auto start = text_data.size();
		auto length = text.length();
		text_data.resize(start + length, char(0));
		std::memcpy(text_data.data() + start, text.data(), length);
		return text_tag{dcon::text_key(uint16_t(start)), uint16_t(length) };
	}

	text_tag state::add_unique_to_pool(std::string_view text) {
		auto search_result = std::search(text_data.begin(), text_data.end(), std::boyer_moore_horspool_searcher(text.begin(), text.end()));
		if(search_result != text_data.end()) {
			return text_tag{ dcon::text_key(uint16_t(search_result - text_data.begin())), uint16_t(text.length()) };
		} else {
			return add_to_pool(text);
		}
	}
}