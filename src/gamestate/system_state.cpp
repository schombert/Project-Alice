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
		if(is_dragging) {
			is_dragging = false;
			on_drag_finished(x, y, mod);
		}
	}
	void state::on_mouse_move(int32_t x, int32_t y, key_modifiers mod) {

	}
	void state::on_mouse_drag(int32_t x, int32_t y, key_modifiers mod) { // called when the left button is held down
		is_dragging = true;
	}
	void state::on_drag_finished(int32_t x, int32_t y, key_modifiers mod) { // called when the left button is released after one or more drag events
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
		glClearColor(0.5, 0.5, 0.5, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(open_gl.ui_shader_program);
		glUniform1f(ogl::parameters::screen_width, float(x_size));
		glUniform1f(ogl::parameters::screen_height, float(y_size));
	}

	//
	// string pool functions
	//

	std::string_view state::to_string_view(dcon::text_key tag) const {
		if(!tag)
			return std::string_view();
		auto start_position = text_data.data() + tag.index();
		auto data_size = text_data.size();
		auto end_position = start_position;
		for(; end_position < text_data.data() + data_size; ++end_position) {
			if(*end_position == 0)
				break;
		}
		return std::string_view(text_data.data() + tag.index(), size_t(end_position - start_position));
	}

	dcon::text_key state::add_to_pool_lowercase(std::string const& text) {
		auto res = add_to_pool(text);
		for(auto i = 0; i < int32_t(text.length()); ++i) {
			text_data[res.index() + i] = char(tolower(text_data[res.index() + i]));
		}
		return res;
	}
	dcon::text_key state::add_to_pool_lowercase(std::string_view text) {
		auto res = add_to_pool(text);
		for(auto i = 0; i < int32_t(text.length()); ++i) {
			text_data[res.index() + i] = char(tolower(text_data[res.index() + i]));
		}
		return res;
	}
	dcon::text_key state::add_to_pool(std::string const& text) {
		auto start = text_data.size();
		auto size = text.size();
		if(size == 0)
			return dcon::text_key();
		text_data.resize(start + size + 1, char(0));
		std::memcpy(text_data.data() + start, text.c_str(), size + 1);
		return dcon::text_key(uint32_t(start));
	}
	dcon::text_key state::add_to_pool(std::string_view text) {
		auto start = text_data.size();
		auto length = text.length();
		text_data.resize(start + length + 1, char(0));
		std::memcpy(text_data.data() + start, text.data(), length);
		text_data.back() = 0;
		return dcon::text_key(uint32_t(start));
	}

	dcon::text_key state::add_unique_to_pool(std::string const& text) {
		auto search_result = std::search(text_data.data(), text_data.data() + text_data.size(), std::boyer_moore_horspool_searcher(text.c_str(), text.c_str() + text.length() + 1));
		if(search_result != text_data.data() + text_data.size()) {
			return dcon::text_key(uint32_t(search_result - text_data.data()));
		} else {
			return add_to_pool(text);
		}
	}
}
