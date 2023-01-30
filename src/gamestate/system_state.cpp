#include "system_state.hpp"
#include "opengl_wrapper.hpp"
#include "window.hpp"
#include "gui_element_base.hpp"
#include <algorithm>
#include <functional>
#include "parsers_declarations.hpp"

namespace sys {
	//
	// window event functions
	//

	void state::on_rbutton_down(int32_t x, int32_t y, key_modifiers mod) {
		// TODO: look at return value
		ui_state.root->impl_on_rbutton_down(*this, int32_t(x / user_settings.ui_scale), int32_t(y / user_settings.ui_scale), mod);
	}
	void state::on_lbutton_down(int32_t x, int32_t y, key_modifiers mod) {
		// TODO: look at return value
		ui_state.root->impl_on_lbutton_down(*this, int32_t(x / user_settings.ui_scale), int32_t(y / user_settings.ui_scale), mod);
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
		// TODO figure out tooltips
	}
	void state::on_mouse_drag(int32_t x, int32_t y, key_modifiers mod) { // called when the left button is held down
		is_dragging = true;

		if(ui_state.drag_target)
			ui_state.drag_target->on_drag(*this,
				int32_t(mouse_x_position / user_settings.ui_scale), int32_t(mouse_y_position / user_settings.ui_scale),
				int32_t(x / user_settings.ui_scale), int32_t(y / user_settings.ui_scale),
				mod);
	}
	void state::on_drag_finished(int32_t x, int32_t y, key_modifiers mod) { // called when the left button is released after one or more drag events
		if(ui_state.drag_target) {
			ui_state.drag_target->on_drag_finish(*this);
			ui_state.drag_target = nullptr;
		}
	}
	void state::on_resize(int32_t x, int32_t y, window_state win_state) {
		if(win_state != window_state::minimized) {
			ui_state.root->base_data.size.x = int16_t(x / user_settings.ui_scale);
			ui_state.root->base_data.size.y = int16_t(y / user_settings.ui_scale);
		}
	}
	void state::on_mouse_wheel(int32_t x, int32_t y, key_modifiers mod, float amount) { // an amount of 1.0 is one "click" of the wheel
		// TODO: look at return value
		auto r = ui_state.root->impl_on_scroll(*this, int32_t(x / user_settings.ui_scale), int32_t(y / user_settings.ui_scale), amount, mod);
		if(r != ui::message_result::consumed) {
			// TODO Settings for making zooming the map faster
			map_zoom *= 1.f + amount / 5.f;
		}
	}
	void state::on_key_down(virtual_key keycode, key_modifiers mod) {
		if(!ui_state.edit_target) {
			if(ui_state.root->impl_on_key_down(*this, keycode, mod) != ui::message_result::consumed) {
				if(keycode == virtual_key::ESCAPE) {
					ui::show_main_menu(*this);
				} else if(keycode == virtual_key::A) {
					map_x_vel += (1.f / map_x_size) * (1.f / map_zoom) * 0.0125f;
				} else if(keycode == virtual_key::D) {
					map_x_vel -= (1.f / map_x_size) * (1.f / map_zoom) * 0.0125f;
				} else if(keycode == virtual_key::W) {
					map_y_vel -= (1.f / map_y_size) * (1.f / map_zoom) * 0.0125f;
				} else if(keycode == virtual_key::S) {
					map_y_vel += (1.f / map_y_size) * (1.f / map_zoom) * 0.0125f;
				} else if(keycode == virtual_key::Q) {
					map_zoom *= 1.1f;
				} else if(keycode == virtual_key::E) {
					map_zoom *= 0.9f;
				}
			}
		}
	}
	void state::on_key_up(virtual_key keycode, key_modifiers mod) {

	}
	void state::on_text(char c) { // c is win1250 codepage value
		if(ui_state.edit_target)
			ui_state.edit_target->on_text(*this, c);
	}
	void state::render() { // called to render the frame may (and should) delay returning until the frame is rendered, including waiting for vsync
		glClearColor(0.5, 0.5, 0.5, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// TODO: Obtain map size from the bitmap size?
		glUseProgram(open_gl.map_shader_program);
		glUniform2f(0, map_x_pos, map_y_pos);
		glUniform2f(1, GLfloat(map_x_size), GLfloat(map_y_size));
		glUniform1f(2, map_zoom);
		glDisable(GL_BLEND);

		glViewport(0, 0, x_size, y_size);
		glDepthRange(-1.0, 1.0);

		ogl::render_map(*this);

		map_x_pos += map_x_vel;
		map_y_pos += map_y_vel;
		auto const velocity_fn = [this](auto& v, auto const v_scale) {
			auto const stop_movement_threshold = (1.f / map_zoom) * 0.0001f;
			v *= 0.975f;

			if(v > 0.f && v < v_scale * stop_movement_threshold) {
				v = 0.f;
			} else if(v < 0.f && v > -v_scale * stop_movement_threshold) {
				v = 0.f;
			}
		};
		velocity_fn(map_x_vel, 1.f / map_x_size);
		velocity_fn(map_y_vel, 1.f / map_y_size);

		// UI rendering
		glUseProgram(open_gl.ui_shader_program);
		glUniform1f(ogl::parameters::screen_width, float(x_size) / user_settings.ui_scale);
		glUniform1f(ogl::parameters::screen_height, float(y_size) / user_settings.ui_scale);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glViewport(0, 0, x_size, y_size);
		glDepthRange(-1.0, 1.0);

		ui_state.under_mouse = ui_state.root->impl_probe_mouse(*this, int32_t(mouse_x_position / user_settings.ui_scale), int32_t(mouse_y_position / user_settings.ui_scale));
		ui_state.root->impl_render(*this, 0, 0);
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


	void state::save_user_settings() const {
		auto settings_location = simple_fs::get_or_create_settings_directory();
		simple_fs::write_file(settings_location, NATIVE("user_settings.dat"), reinterpret_cast<char const*>(&user_settings), uint32_t(sizeof(user_settings_s)));
	}
	void state::load_user_settings() {
		auto settings_location = simple_fs::get_or_create_settings_directory();
		auto settings_file = open_file(settings_location, NATIVE("user_settings.dat"));
		if(settings_file) {
			auto content = view_contents(*settings_file);
			std::memcpy(&user_settings, content.data, std::min(uint32_t(sizeof(user_settings_s)), content.file_size));

			user_settings.interface_volume = std::clamp(user_settings.interface_volume, 0.0f, 1.0f);
			user_settings.music_volume = std::clamp(user_settings.music_volume, 0.0f, 1.0f);
			user_settings.effects_volume = std::clamp(user_settings.effects_volume, 0.0f, 1.0f);
			user_settings.master_volume = std::clamp(user_settings.master_volume, 0.0f, 1.0f);
		}
	}


	void state::update_ui_scale(float new_scale) {
		user_settings.ui_scale = new_scale;
		// TODO move windows
	}

	void state::load_scenario_data() {
		parsers::error_handler err("");

		text::load_text_data(*this, 2); // 2 = English
		ui::load_text_gui_definitions(*this, err);

		auto root = get_root(common_fs);
		auto common = open_directory(root, NATIVE("common"));

		parsers::scenario_building_context context(*this);
		// Read national tags from countries.txt
		{
			auto countries = open_file(common, NATIVE("countries.txt"));
			if(countries) {
				auto content = view_contents(*countries);
				err.file_name = "countries.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_national_identity_file(gen, err, context);
			}
		}
		// read religions from religion.txt
		{
			auto religion = open_file(common, NATIVE("religion.txt"));
			if(religion) {
				auto content = view_contents(*religion);
				err.file_name = "religion.txt";
				parsers::token_generator gen(content.data, content.data + content.file_size);
				parsers::parse_religion_file(gen, err, context);
			}
		}

		// TODO do something with err
	}
}
